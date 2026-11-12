use std::collections::HashMap;
use std::io;
use std::net::SocketAddr;
use std::sync::{Arc, Mutex};
use std::thread::JoinHandle;

use axum::{
    Router,
    extract::{
        State,
        ws::{CloseFrame, Message, WebSocket, WebSocketUpgrade},
    },
    response::IntoResponse,
    routing::get,
};
use bevy::prelude::Resource;
use crossbeam_channel::Sender;
use tokio::sync::{mpsc, oneshot};

use crate::bridge::ServerEvent;
use crate::moves::Move;

const MAX_ENGINES: usize = 2;
const REJECT_CLOSE_CODE: u16 = 1000;
const REJECT_REASON: &str = "Server is full. Two engines are already connected.";

type EngineId = u8;

struct ServerState {
    engines: Mutex<HashMap<EngineId, mpsc::UnboundedSender<Message>>>,
    events: Sender<ServerEvent>,
}

impl ServerState {
    fn new(events: Sender<ServerEvent>) -> Self {
        Self {
            engines: Mutex::new(HashMap::new()),
            events,
        }
    }

    fn log(&self, line: impl Into<String>) {
        let line = line.into();
        eprintln!("[server] {line}");
        let _ = self.events.send(ServerEvent::Log(line));
    }
}

struct ServerHandle {
    shutdown: Option<oneshot::Sender<()>>,
    thread: Option<JoinHandle<()>>,
}

impl ServerHandle {
    fn stop(mut self) {
        if let Some(shutdown) = self.shutdown.take() {
            let _ = shutdown.send(());
        }
        if let Some(thread) = self.thread.take() {
            let _ = thread.join();
        }
    }
}

/// Live websocket server owned as a Bevy resource: current address, last
/// startup error, and a handle used to stop/restart it at runtime.
#[derive(Resource)]
pub struct ServerControl {
    pub addr: SocketAddr,
    pub last_error: Option<String>,
    events: Sender<ServerEvent>,
    handle: Option<ServerHandle>,
}

impl ServerControl {
    /// Bind first (so a busy port fails synchronously), then spawn the
    /// dedicated OS thread running its own Tokio runtime.
    pub fn start(addr: SocketAddr, events: Sender<ServerEvent>) -> Self {
        let mut control = Self {
            addr,
            last_error: None,
            events,
            handle: None,
        };
        let result = bind_and_spawn(addr, control.events.clone());
        control.replace_handle(addr, result);
        control
    }

    /// Swap in a newly spawned server; the old one is stopped only after the
    /// new listener bound successfully, so a failed restart keeps it running.
    fn replace_handle(&mut self, addr: SocketAddr, result: io::Result<ServerHandle>) {
        match result {
            Ok(handle) => {
                if let Some(old) = self.handle.take() {
                    old.stop();
                }
                self.addr = addr;
                self.last_error = None;
                self.handle = Some(handle);
            }
            Err(err) => {
                let line = format!("failed to start websocket server on {addr}: {err}");
                eprintln!("[server] {line}");
                let _ = self.events.send(ServerEvent::Log(line.clone()));
                self.last_error = Some(line);
            }
        }
    }

    pub fn restart(&mut self, addr: SocketAddr) {
        let result = bind_and_spawn(addr, self.events.clone());
        self.replace_handle(addr, result);
    }
}

fn bind_and_spawn(addr: SocketAddr, events: Sender<ServerEvent>) -> io::Result<ServerHandle> {
    let listener = std::net::TcpListener::bind(addr)?;
    listener.set_nonblocking(true)?;

    let (shutdown_tx, shutdown_rx) = oneshot::channel();
    let thread = std::thread::Builder::new()
        .name("engine-server".into())
        .spawn(move || {
            let runtime = match tokio::runtime::Runtime::new() {
                Ok(runtime) => runtime,
                Err(err) => {
                    eprintln!("[server] failed to build engine server runtime: {err}");
                    return;
                }
            };
            runtime.block_on(async move {
                let listener = match tokio::net::TcpListener::from_std(listener) {
                    Ok(listener) => listener,
                    Err(err) => {
                        eprintln!("[server] failed to convert listener: {err}");
                        return;
                    }
                };

                let state = Arc::new(ServerState::new(events));
                state.log(format!(
                    "Engine Server is running! Engines should connect to: ws://{addr}"
                ));

                let app = Router::new()
                    .route("/", get(ws_upgrade))
                    .fallback(ws_upgrade)
                    .with_state(state);

                tokio::select! {
                    result = axum::serve(listener, app) => {
                        if let Err(err) = result {
                            eprintln!("[server] server terminated: {err}");
                        }
                    }
                    _ = shutdown_rx => {}
                }
            });
        })?;

    Ok(ServerHandle {
        shutdown: Some(shutdown_tx),
        thread: Some(thread),
    })
}

async fn ws_upgrade(
    ws: WebSocketUpgrade,
    State(state): State<Arc<ServerState>>,
) -> impl IntoResponse {
    ws.on_upgrade(move |socket| handle_engine(socket, state))
}

enum EngineEvent {
    Incoming(Option<Result<Message, axum::Error>>),
    Outgoing(Option<Message>),
}

async fn handle_engine(mut socket: WebSocket, state: Arc<ServerState>) {
    let registration = {
        let mut engines = state.engines.lock().expect("engine registry lock");
        if engines.len() >= MAX_ENGINES {
            None
        } else {
            let engine_id = (1..=MAX_ENGINES as EngineId)
                .find(|id| !engines.contains_key(id))
                .expect("free engine slot");
            let (tx, rx) = mpsc::unbounded_channel();
            engines.insert(engine_id, tx);
            Some((engine_id, rx))
        }
    };

    let Some((engine_id, mut rx)) = registration else {
        let _ = socket
            .send(Message::Close(Some(CloseFrame {
                code: REJECT_CLOSE_CODE,
                reason: REJECT_REASON.into(),
            })))
            .await;
        state.log(format!("Rejected connection: {REJECT_REASON}"));
        return;
    };

    state.log(format!("Engine {engine_id} connected"));

    loop {
        // The select statement owns both futures; once it resolves they are
        // dropped, releasing the `&mut socket` borrow for the handler below.
        let event = tokio::select! {
            incoming = socket.recv() => EngineEvent::Incoming(incoming),
            outgoing = rx.recv() => EngineEvent::Outgoing(outgoing),
        };

        match event {
            EngineEvent::Incoming(Some(Ok(message))) => {
                let payload = match &message {
                    Message::Text(text) => Some(text.to_string()),
                    Message::Binary(bytes) => Some(String::from_utf8_lossy(bytes).into_owned()),
                    _ => None,
                };
                let Some(raw) = payload else {
                    continue;
                };

                state.log(format!("Move received from Engine {engine_id}: {raw}"));

                {
                    let engines = state.engines.lock().expect("engine registry lock");
                    for (other_id, tx) in engines.iter() {
                        if *other_id != engine_id {
                            let _ = tx.send(message.clone());
                        }
                    }
                }

                match raw.parse::<Move>() {
                    Ok(movement) => {
                        if state.events.send(ServerEvent::Move(movement)).is_err() {
                            state.log(format!("UI channel closed, dropping move"));
                            break;
                        }
                    }
                    Err(err) => state.log(format!("{err}")),
                }
            }
            EngineEvent::Incoming(Some(Err(err))) => {
                state.log(format!("Engine {engine_id} error: {err}"));
                break;
            }
            EngineEvent::Incoming(None) => break,
            EngineEvent::Outgoing(Some(message)) => {
                if socket.send(message).await.is_err() {
                    break;
                }
            }
            EngineEvent::Outgoing(None) => break,
        }
    }

    state
        .engines
        .lock()
        .expect("engine registry lock")
        .remove(&engine_id);
    state.log(format!("Engine {engine_id} disconnected"));
}
