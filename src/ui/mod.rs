use bevy::prelude::*;
use bevy_egui::{EguiContexts, egui};
use egui_dock::{DockArea, DockState, Style, TabViewer};

use crate::bridge::Logs;
use crate::game::GameState;
use crate::server::ServerControl;

pub mod board;
pub mod controls;
pub mod logs;

#[derive(Clone, Copy, Debug, PartialEq, Eq, Hash)]
pub enum Tab {
    Board,
    EngineLogs,
    Controls,
}

/// The egui_dock layout as a Bevy resource (foreign types can't derive
/// `Resource` directly).
#[derive(Resource)]
pub struct DockLayout(pub DockState<Tab>);

pub struct AppTabViewer<'a> {
    pub game: &'a mut GameState,
    pub textures: &'a mut board::PieceTextures,
    pub server: &'a mut ServerControl,
    pub logs: &'a mut Logs,
    pub controls: &'a mut controls::ControlsState,
}

impl TabViewer for AppTabViewer<'_> {
    type Tab = Tab;

    fn id(&mut self, tab: &mut Self::Tab) -> egui::Id {
        egui::Id::new(*tab)
    }

    fn title(&mut self, tab: &mut Self::Tab) -> egui::WidgetText {
        match tab {
            Tab::Board => "Board",
            Tab::EngineLogs => "Engine Logs",
            Tab::Controls => "Controls",
        }
        .into()
    }

    fn ui(&mut self, ui: &mut egui::Ui, tab: &mut Self::Tab) {
        match tab {
            Tab::Board => {
                board::show_board(ui, self.game, self.textures, self.server.addr);
            }
            Tab::EngineLogs => {
                logs::show_logs(ui, self.logs);
            }
            Tab::Controls => {
                controls::show_controls(ui, self.controls, self.server, self.game, self.logs);
            }
        }
    }

    fn is_closeable(&self, _tab: &Self::Tab) -> bool {
        false
    }
}

/// All three tabs live in a single pane: the active tab (Board by default)
/// gets the whole content area, and the tab bar switches between them.
pub fn initial_dock_state() -> DockState<Tab> {
    let dock_state = DockState::new(vec![Tab::Board, Tab::Controls, Tab::EngineLogs]);
    dock_state
}

/// Single full-screen egui pass: central panel hosts the dock layout.
pub fn render_dock_system(
    mut contexts: EguiContexts,
    mut layout: ResMut<DockLayout>,
    mut game: ResMut<GameState>,
    mut server: ResMut<ServerControl>,
    mut logs: ResMut<Logs>,
    mut controls: ResMut<controls::ControlsState>,
    mut textures: Local<board::PieceTextures>,
) -> Result {
    let ctx = contexts.ctx_mut()?;
    textures.ensure_loaded(ctx);

    // egui 0.36 renders top-level panels against a root Ui instead of the Context.
    let mut viewport_ui = egui::Ui::new(
        ctx.clone(),
        "viewport".into(),
        egui::UiBuilder::new()
            .layer_id(egui::LayerId::background())
            .max_rect(ctx.viewport_rect()),
    );
    let mut viewer = AppTabViewer {
        game: &mut game,
        textures: &mut textures,
        server: &mut server,
        logs: &mut logs,
        controls: &mut controls,
    };
    egui::CentralPanel::default().show(&mut viewport_ui, |ui| {
        DockArea::new(&mut layout.0)
            .style(Style::from_egui(ui.style().as_ref()))
            .show_add_buttons(false)
            .show_add_popup(false)
            .show_close_buttons(false)
            .show_leaf_close_all_buttons(false)
            .show_leaf_collapse_buttons(false)
            .tab_context_menus(false)
            .draggable_tabs(false)
            .hidable_tab_bars(false)
            .show_tab_name_on_hover(false)
            .show_inside(ui, &mut viewer);
    });
    Ok(())
}
