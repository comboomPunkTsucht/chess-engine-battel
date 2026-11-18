use gpui_kit::{
    Context, StatefulInteractiveElement, InteractiveElement as _, IntoElement, ParentElement as _, Styled as _, div,
};
use gpui_kit::component::{ActiveTheme as _, label::Label};

use crate::nord::FONT_FAMILY_MONO;
use crate::ui::BattleView;

pub fn render(view: &BattleView, _cx: &mut Context<BattleView>) -> impl IntoElement {
    let logs = &view.logs;
    let scroll = &view.logs_scroll;
    let mono = FONT_FAMILY_MONO;

    div()
        .id("logs")
        .size_full()
        .flex()
        .flex_col()
        .gap_1()
        .p(px_8())
        .overflow_y_scroll()
        .track_scroll(scroll)
        .font_family(mono)
        .text_sm()
        .children(if logs.0.is_empty() {
            vec![Label::new("No log entries yet.")
                .id("empty")
                .into_any_element()]
        } else {
            logs.0
                .iter()
                .enumerate()
                .map(|(i, line)| {
                    Label::new(line.as_str())
                        .id(format!("log-{i}"))
                        .into_any_element()
                })
                .collect::<Vec<_>>()
        })
}

fn px_8() -> gpui_kit::Pixels {
    gpui_kit::px(8.)
}
