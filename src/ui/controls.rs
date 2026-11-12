use std::net::SocketAddr;

use bevy::prelude::*;
use bevy_egui::egui;

use crate::bridge::Logs;
use crate::game::GameState;
use crate::nord::{NORD11, NORD14, hex};
use crate::server::ServerControl;

/// Editable server URI plus the last Apply result, shown in the Controls tab.
#[derive(Resource, Default)]
pub struct ControlsState {
    pub uri_text: String,
    pub status: Option<String>,
    pub status_ok: bool,
}

pub fn show_controls(
    ui: &mut egui::Ui,
    state: &mut ControlsState,
    server: &mut ServerControl,
    game: &mut GameState,
    logs: &mut Logs,
) {
    ui.heading("Server");
    ui.horizontal(|ui| {
        ui.label("WebSocket URI:");
        ui.add(egui::TextEdit::singleline(&mut state.uri_text).desired_width(240.0));
        if ui.button("Apply").clicked() {
            apply(state, server, game, logs);
        }
    });
    if let Some(status) = &state.status {
        let color = if state.status_ok {
            hex(NORD14)
        } else {
            hex(NORD11)
        };
        ui.colored_label(color, status);
    }
    ui.label("Changing the URI restarts the WebSocket server and resets the board.");

    ui.separator();
    ui.heading("About");
    ui.label(format!(
        "Chess Engine Battle v{}",
        env!("CARGO_PKG_VERSION")
    ));
    ui.label("Bevy + bevy_egui + egui_dock + Axum");
    ui.hyperlink_to(
        "Repository",
        "https://github.com/comboomPunkTsucht/chess-engine-battel",
    );
    ui.label("Nord theme · CaskaydiaCove Nerd Font");
}

fn apply(
    state: &mut ControlsState,
    server: &mut ServerControl,
    game: &mut GameState,
    logs: &mut Logs,
) {
    let trimmed = state.uri_text.trim();
    let addr_text = trimmed.strip_prefix("ws://").unwrap_or(trimmed);

    let Ok(addr) = addr_text.parse::<SocketAddr>() else {
        state.status = Some(format!("Invalid address: {trimmed}"));
        state.status_ok = false;
        return;
    };

    if addr == server.addr && server.last_error.is_none() {
        state.status = Some(format!("Already running on ws://{addr}"));
        state.status_ok = true;
        return;
    }

    server.restart(addr);

    if let Some(error) = server.last_error.clone() {
        state.status = Some(error);
        state.status_ok = false;
        return;
    }

    *game = GameState::new();
    logs.push(format!("Server restarted on ws://{addr}"));
    state.uri_text = format!("ws://{addr}");
    state.status = Some(format!("Restarted on ws://{addr}"));
    state.status_ok = true;
}
