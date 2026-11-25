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
use crossbeam_channel::Sender;
use futures_util::StreamExt;
use futures_util::SinkExt;
use tokio::sync::{mpsc, oneshot};

use crate::bridge::ServerEvent;
use crate::moves::{Move, MoveType};
use crate::piece::PieceType;

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

/// Live websocket server: current address, last startup error, and a handle
/// used to stop/restart it at runtime.
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
        control.restart(addr);
        control
    }

    /// Stop the current server (if any) and start a new one on `addr`.
    pub fn restart(&mut self, addr: SocketAddr) {
        if let Some(handle) = self.handle.take() {
            handle.stop();
        }
        self.last_error = None;

        match try_bind(addr) {
            Ok(listener) => {
                self.addr = listener.local_addr().unwrap_or(addr);
                let (shutdown_tx, shutdown_rx) = oneshot::channel();
                let events = self.events.clone();
                let state = Arc::new(ServerState::new(events));
                let thread = std::thread::spawn(move || {
                    let runtime = match tokio::runtime::Builder::new_multi_thread()
                        .enable_all()
                        .build()
                    {
                        Ok(runtime) => runtime,
                        Err(err) => {
                            eprintln!("[server] failed to build tokio runtime: {err}");
                            return;
                        }
                    };
                    runtime.block_on(async move {
                        // Axum's `serve` requires a `tokio::net::TcpListener`, not `std::net::TcpListener`.
                        // Convert the std listener by setting it nonblocking first.
                        let std_listener = listener;
                        let _ = std_listener.set_nonblocking(true);
                        let tcp_listener = tokio::net::TcpListener::from_std(std_listener).unwrap();
                        let app = Router::new()
                            .route("/ws", get(ws_handler))
                            .with_state(state);
                        let server = axum::serve(tcp_listener, app).with_graceful_shutdown(async {
                            let _ = shutdown_rx.await;
                        });
                        if let Err(err) = server.await {
                            eprintln!("[server] server error: {err}");
                        }
                    });
                });
                self.handle = Some(ServerHandle {
                    shutdown: Some(shutdown_tx),
                    thread: Some(thread),
                });
                eprintln!("[server] listening on ws://{}", self.addr);
            }
            Err(err) => {
                self.last_error = Some(format!("Failed to bind {addr}: {err}"));
                eprintln!("[server] {}", self.last_error.as_deref().unwrap_or_default());
            }
        }
    }
}

impl Drop for ServerControl {
    fn drop(&mut self) {
        if let Some(handle) = self.handle.take() {
            handle.stop();
        }
    }
}

fn try_bind(addr: SocketAddr) -> io::Result<std::net::TcpListener> {
    std::net::TcpListener::bind(addr)
}

async fn ws_handler(
    ws: WebSocketUpgrade,
    State(state): State<Arc<ServerState>>,
) -> impl IntoResponse {
    let engine_id = allocate_engine_id(&state);
    let Some(engine_id) = engine_id else {
        return ws
            .on_upgrade(|mut socket| async move {
                let _ = socket
                    .send(Message::Close(Some(CloseFrame {
                        code: REJECT_CLOSE_CODE,
                        reason: REJECT_REASON.into(),
                    })))
                    .await;
            })
            .into_response();
    };

    ws.on_upgrade(move |socket| handle_socket(socket, engine_id, state))
        .into_response()
}

fn allocate_engine_id(state: &ServerState) -> Option<EngineId> {
    let mut engines = state.engines.lock().ok()?;
    if engines.len() >= MAX_ENGINES {
        return None;
    }
    let id = (0..MAX_ENGINES as u8).find(|id| !engines.contains_key(id))?;
    let (tx, _rx) = mpsc::unbounded_channel();
    engines.insert(id, tx);
    Some(id)
}

async fn handle_socket(socket: WebSocket, engine_id: EngineId, state: Arc<ServerState>) {
    let (mut sender, mut receiver) = socket.split();
    let (tx, mut rx) = mpsc::unbounded_channel::<Message>();

    {
        let mut engines = match state.engines.lock() {
            Ok(engines) => engines,
            Err(_) => return,
        };
        engines.insert(engine_id, tx);
    }

    state.log(format!("Engine {engine_id} connected"));

    let send_task = tokio::spawn(async move {
        while let Some(msg) = rx.recv().await {
            if sender.send(msg).await.is_err() {
                break;
            }
        }
    });

    while let Some(Ok(msg)) = receiver.next().await {
        match msg {
            Message::Text(text) => {
                if let Some(movement) = parse_move(&text) {
                    let _ = state.events.send(ServerEvent::Move(movement));
                } else {
                    state.log(format!("Engine {engine_id} sent unparsable message: {text}"));
                }
            }
            Message::Close(_) => break,
            _ => {}
        }
    }

    send_task.abort();
    if let Ok(mut engines) = state.engines.lock() {
        engines.remove(&engine_id);
    }
    state.log(format!("Engine {engine_id} disconnected"));
}

fn parse_move(text: &str) -> Option<Move> {
    let text = text.trim();
    if text.len() != 4 && text.len() != 5 {
        return None;
    }
    let bytes = text.as_bytes();
    let from_file = (bytes[0] as u8 - b'a') as usize;
    let from_rank = (bytes[1] as u8 - b'1') as usize;
    let to_file = (bytes[2] as u8 - b'a') as usize;
    let to_rank = (bytes[3] as u8 - b'1') as usize;
    let from = from_rank * 8 + from_file;
    let to = to_rank * 8 + to_file;
    let promo = if text.len() == 5 {
        match bytes[4] as char {
            'q' | 'Q' => PieceTypeProxy::Queen,
            'r' | 'R' => PieceTypeProxy::Rook,
            'b' | 'B' => PieceTypeProxy::Bishop,
            'n' | 'N' => PieceTypeProxy::Knight,
            _ => return None,
        }
    } else {
        PieceTypeProxy::None
    };
    Some(Move::make_move(from, to, MoveType::Normal, PieceType::None, promo.into(), 0, 0))
}

// Local proxy so parse_move can name promotion pieces without importing the
// full piece module into the server path.
#[derive(Clone, Copy)]
enum PieceTypeProxy {
    None,
    Queen,
    Rook,
    Bishop,
    Knight,
}

impl From<PieceTypeProxy> for crate::piece::PieceType {
    fn from(value: PieceTypeProxy) -> Self {
        use crate::piece::PieceType;
        match value {
            PieceTypeProxy::None => PieceType::None,
            PieceTypeProxy::Queen => PieceType::Queen,
            PieceTypeProxy::Rook => PieceType::Rook,
            PieceTypeProxy::Bishop => PieceType::Bishop,
            PieceTypeProxy::Knight => PieceType::Knight,
        }
    }
}
