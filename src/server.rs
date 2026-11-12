use std::collections::HashMap;
use std::net::SocketAddr;
use std::sync::{Arc, Mutex};

use axum::{
    Router,
    extract::{
        State,
        ws::{CloseFrame, Message, WebSocket, WebSocketUpgrade},
    },
    response::IntoResponse,
    routing::get,
};
use tokio::sync::mpsc::{self, UnboundedSender};

use crate::moves::Move;

const MAX_ENGINES: usize = 2;
const REJECT_CLOSE_CODE: u16 = 1000;
const REJECT_REASON: &str = "Server is full. Two engines are already connected.";

type EngineId = u8;

struct ServerState {
    engines: Mutex<HashMap<EngineId, UnboundedSender<Message>>>,
    to_ui: UnboundedSender<Move>,
}

impl ServerState {
    fn new(to_ui: UnboundedSender<Move>) -> Self {
        Self {
            engines: Mutex::new(HashMap::new()),
            to_ui,
        }
    }
}

/// Spawn the engine websocket server on a dedicated OS thread running its own
/// multi-threaded Tokio runtime. Every text/binary frame is parsed as a
/// `Move`, forwarded to the other connected engine, and delivered to the UI
/// over `to_ui`.
pub fn spawn_engine_server(
    addr: SocketAddr,
    to_ui: UnboundedSender<Move>,
) -> std::thread::JoinHandle<()> {
    std::thread::Builder::new()
        .name("engine-server".into())
        .spawn(move || {
            let runtime = tokio::runtime::Builder::new_multi_thread()
                .worker_threads(2)
                .enable_all()
                .thread_name("engine-server-worker")
                .build()
                .expect("failed to build engine server runtime");

            runtime.block_on(async move {
                let state = Arc::new(ServerState::new(to_ui));
                let app = Router::new()
                    .route("/", get(ws_upgrade))
                    .fallback(ws_upgrade)
                    .with_state(state);

                match tokio::net::TcpListener::bind(addr).await {
                    Ok(listener) => {
                        eprintln!(
                            "[server] Engine Server is running! Engines should connect to: ws://{addr}"
                        );
                        if let Err(err) = axum::serve(listener, app).await {
                            eprintln!("[server] server terminated: {err}");
                        }
                    }
                    Err(err) => {
                        eprintln!("[server] failed to start websocket server on {addr}: {err}");
                    }
                }
            });
        })
        .expect("failed to spawn engine server thread")
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
        eprintln!("[server] Rejected connection: {REJECT_REASON}");
        return;
    };

    eprintln!("[server] Engine {engine_id} connected");

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

                eprintln!("[server] Move received from Engine {engine_id}: {raw}");

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
                        if state.to_ui.send(movement).is_err() {
                            eprintln!("[server] UI channel closed, dropping move {movement}");
                            break;
                        }
                    }
                    Err(err) => eprintln!("[server] {err}"),
                }
            }
            EngineEvent::Incoming(Some(Err(err))) => {
                eprintln!("[server] Engine {engine_id} error: {err}");
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
    eprintln!("[server] Engine {engine_id} disconnected");
}
