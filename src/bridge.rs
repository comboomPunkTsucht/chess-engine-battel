use crossbeam_channel::Receiver;

use crate::game::GameState;
use crate::moves::Move;

const MAX_LOG_LINES: usize = 1000;

/// Events pushed by the websocket server thread and polled by the UI each frame.
pub enum ServerEvent {
    Move(Move),
    Log(String),
}

/// The crossbeam `Receiver` owned by the view (foreign type, no wrapper).
pub struct EventReceiver(pub Receiver<ServerEvent>);

/// Ring-buffer of server log lines shown in the Engine Logs tab.
#[derive(Default)]
pub struct Logs(pub Vec<String>);

impl Logs {
    pub fn push(&mut self, line: String) {
        self.0.push(line);
        if self.0.len() > MAX_LOG_LINES {
            self.0.remove(0);
        }
    }
}

/// Non-blocking drain of the crossbeam receiver every frame: moves mutate the
/// board, log lines accumulate for the UI.
pub fn drain(receiver: &EventReceiver, game: &mut GameState, logs: &mut Logs) {
    while let Ok(event) = receiver.0.try_recv() {
        match event {
            ServerEvent::Move(movement) => game.do_move(movement),
            ServerEvent::Log(line) => logs.push(line),
        }
    }
}
