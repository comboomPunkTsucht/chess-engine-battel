use std::time::Duration;

use gpui_kit::{
    App, Context, Entity, IntoElement, ParentElement as _, Render, ScrollHandle, Styled as _,
    Window, div,
};
use gpui_kit::component::{
    ActiveTheme as _, input::InputState,
    tab::{Tab, TabBar},
};

use crate::bridge::{self, EventReceiver, Logs};
use crate::game::GameState;
use crate::server::ServerControl;
use crate::ui::controls::ControlsState;

pub mod board;
pub mod controls;
pub mod logs;
pub mod menus;

pub(crate) fn install_menus(cx: &mut App) {
    menus::install_menus(cx);
}

const POLL_INTERVAL: Duration = Duration::from_millis(16);

pub struct BattleView {
    game: GameState,
    receiver: EventReceiver,
    logs: Logs,
    server: ServerControl,
    controls: ControlsState,
    uri_input: Entity<InputState>,
    logs_scroll: ScrollHandle,
    selected_tab: usize,
}

enum PollStep {
    Idle,
    Closed,
}

impl BattleView {
    pub fn new(
        receiver: EventReceiver,
        server: ServerControl,
        uri_input: Entity<InputState>,
        cx: &mut Context<Self>,
    ) -> Self {
        let uri_text = format!("ws://{}", server.addr);
        let view = Self {
            game: GameState::new(),
            receiver,
            logs: Logs::default(),
            server,
            controls: ControlsState {
                uri_text,
                status: None,
                status_ok: false,
            },
            uri_input,
            logs_scroll: ScrollHandle::new(),
            selected_tab: 0,
        };

        cx.spawn(async move |this, cx| {
            loop {
                let step = match this.update(cx, |view, cx| {
                    let logs_before = view.logs.0.len();
                    let moves_before = view.game.history.len();
                    bridge::drain(&view.receiver, &mut view.game, &mut view.logs);
                    let logs_grew = view.logs.0.len() > logs_before;
                    let moved = view.game.history.len() != moves_before;
                    if logs_grew {
                        view.logs_scroll.scroll_to_bottom();
                    }
                    if logs_grew || moved {
                        cx.notify();
                    }
                    PollStep::Idle
                }) {
                    Ok(step) => step,
                    Err(_) => PollStep::Closed,
                };
                if matches!(step, PollStep::Closed) {
                    return;
                }
                cx.background_executor().timer(POLL_INTERVAL).await;
            }
        })
        .detach();

        view
    }
}

impl Render for BattleView {
    fn render(&mut self, _window: &mut Window, cx: &mut Context<Self>) -> impl IntoElement {
        let content = match self.selected_tab {
            0 => board::render(self, cx).into_any_element(),
            1 => controls::render(self, cx).into_any_element(),
            _ => logs::render(self, cx).into_any_element(),
        };

        div()
            .size_full()
            .flex()
            .flex_col()
            .bg(cx.theme().background)
            .child(
                TabBar::new("tabs")
                    .w_full()
                    .selected_index(self.selected_tab)
                    .on_click(cx.listener(|this, ix: &usize, _, cx| {
                        this.selected_tab = *ix;
                        cx.notify();
                    }))
                    .child(Tab::new().label("Board"))
                    .child(Tab::new().label("Controls"))
                    .child(Tab::new().label("Engine Logs")),
            )
            .child(div().flex_1().min_h_0().overflow_hidden().child(content))
    }
}
