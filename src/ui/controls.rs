use std::net::SocketAddr;

use gpui_kit::{
    Context, px, IntoElement, ParentElement as _, Styled as _, Window, div,
};
use gpui_kit::component::{
    ActiveTheme as _,
    button::Button,
    input::{Input, InputState},
    label::Label,
};

use crate::game::GameState;
use crate::nord::{self, FONT_FAMILY, NORD11, NORD14, NORD6};
use crate::server::ServerControl;
use crate::ui::BattleView;

/// Editable server URI plus the last Apply result, shown in the Controls tab.
#[derive(Default)]
pub struct ControlsState {
    pub uri_text: String,
    pub status: Option<String>,
    pub status_ok: bool,
}

pub fn render(
    view: &BattleView,
    cx: &mut Context<BattleView>,
) -> impl IntoElement {
    let status = view.controls.status.clone();
    let status_ok = view.controls.status_ok;
    let status_color = if status_ok {
        nord::hex(NORD14)
    } else {
        nord::hex(NORD11)
    };
    let foreground = nord::hex(NORD6);

    div()
        .size_full()
        .flex()
        .flex_col()
        .gap_3()
        .p(px(16.))
        .font_family(FONT_FAMILY)
        .text_color(foreground)
        .child(Label::new("Server").text_lg())
        .child(
            div()
                .flex()
                .flex_row()
                .items_center()
                .gap_2()
                .child(Label::new("WebSocket URI:"))
                .child(Input::new(&view.uri_input).w(px(320.)))
                .child(
                    Button::new("apply")
                        .label("Apply")
                        .on_click(cx.listener(|view, _, window, cx| {
                            apply(view, window, cx);
                        })),
                ),
        )
        .children(status.map(|status| {
            Label::new(status).text_color(status_color).id("status")
        }))
        .child(
            Label::new("Changing the URI restarts the WebSocket server and resets the board.")
                .text_sm(),
        )
        .child(div().w_full().h(px(1.)).bg(cx.theme().border))
        .child(Label::new("About").text_lg())
        .child(Label::new(format!(
            "Chess Engine Battle v{}",
            env!("CARGO_PKG_VERSION")
        )))
        .child(Label::new("gpui-kit + Axum"))
        .child(
            Button::new("repo")
                .label("Repository")
                .on_click(|_, _, cx| {
                    cx.open_url("https://github.com/comboomPunkTsucht/chess-engine-battel");
                }),
        )
        .child(Label::new("Nord theme · CaskaydiaCove Nerd Font"))
}

fn apply(view: &mut BattleView, window: &mut Window, cx: &mut Context<BattleView>) {
    let trimmed = view.uri_input.read(cx).value();
    let trimmed = trimmed.trim();
    let addr_text = trimmed.strip_prefix("ws://").unwrap_or(trimmed);

    let Ok(addr) = addr_text.parse::<SocketAddr>() else {
        view.controls.status = Some(format!("Invalid address: {trimmed}"));
        view.controls.status_ok = false;
        cx.notify();
        return;
    };

    if addr == view.server.addr && view.server.last_error.is_none() {
        view.controls.status = Some(format!("Already running on ws://{addr}"));
        view.controls.status_ok = true;
        cx.notify();
        return;
    }

    view.server.restart(addr);

    if let Some(error) = view.server.last_error.clone() {
        view.controls.status = Some(error);
        view.controls.status_ok = false;
        cx.notify();
        return;
    }

    view.game = GameState::new();
    view.logs.push(format!("Server restarted on ws://{addr}"));
    let new_uri = format!("ws://{addr}");
    view.controls.uri_text = new_uri.clone();
    view.controls.status = Some(format!("Restarted on ws://{addr}"));
    view.controls.status_ok = true;
    view.uri_input.update(cx, |state, cx| {
        state.set_value(new_uri, window, cx);
    });
    cx.notify();
}
