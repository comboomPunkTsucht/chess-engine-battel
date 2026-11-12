use std::time::Duration;

use gpui::{
    BorderStyle, Bounds, Context, IntoElement, ParentElement, Render, Rgba, Styled, Timer, Window,
    canvas, div, point, px, quad, size, transparent_black,
};
use tokio::sync::mpsc::{UnboundedReceiver, error::TryRecvError};

use crate::game::{BOARD_DIM, GameState};
use crate::moves::Move;
use crate::nord::{self, NORD1, NORD3, NORD4, NORD_BACKGROUND, NORD_FOREGROUND, NORD_PRIMARY};
use crate::piece::{PieceColor, PieceType};

include!(concat!(env!("OUT_DIR"), "/piece_sprites.rs"));

const POLL_INTERVAL: Duration = Duration::from_millis(16);
const BORDER_WIDTH: f32 = 1.0;
const OVERLAY_INSET: f32 = 8.0;
const FONT_FAMILY: &str = "CaskaydiaCove Nerd Font Propo";

pub struct BattleView {
    game: GameState,
    receiver: UnboundedReceiver<Move>,
    server_uri: String,
}

enum PollStep {
    Progress,
    Idle,
    Closed,
}

impl BattleView {
    pub fn new(
        receiver: UnboundedReceiver<Move>,
        server_uri: String,
        cx: &mut Context<Self>,
    ) -> Self {
        let view = Self {
            game: GameState::new(),
            receiver,
            server_uri,
        };

        cx.spawn(async move |this, cx| {
            loop {
                loop {
                    let step = match this.update(cx, |view, cx| match view.receiver.try_recv() {
                        Ok(movement) => {
                            view.game.do_move(movement);
                            cx.notify();
                            PollStep::Progress
                        }
                        Err(TryRecvError::Empty) => PollStep::Idle,
                        Err(TryRecvError::Disconnected) => PollStep::Closed,
                    }) {
                        Ok(step) => step,
                        Err(_) => PollStep::Closed,
                    };

                    match step {
                        PollStep::Progress => continue,
                        PollStep::Idle => break,
                        PollStep::Closed => return,
                    }
                }

                Timer::after(POLL_INTERVAL).await;
            }
        })
        .detach();

        view
    }
}

fn piece_kind_byte(kind: PieceType) -> Option<u8> {
    match kind {
        PieceType::None => None,
        PieceType::Pawn => Some(1),
        PieceType::Knight => Some(2),
        PieceType::Bishop => Some(3),
        PieceType::Rook => Some(4),
        PieceType::Queen => Some(5),
        PieceType::King => Some(6),
    }
}

/// Raylib `DrawTexturePro` tint: per-channel multiply of sprite RGB by the
/// palette color (white pieces → NORD4, black pieces → NORD3). The source
/// sprite is always the white art; black is only tinted, never a separate asset.
fn tinted_rgb(sprite_rgb: [u8; 3], tint: u32) -> [f32; 3] {
    let [tr, tg, tb, _] = tint.to_be_bytes();
    [
        sprite_rgb[0] as f32 * tr as f32 / 255.0 / 255.0,
        sprite_rgb[1] as f32 * tg as f32 / 255.0 / 255.0,
        sprite_rgb[2] as f32 * tb as f32 / 255.0 / 255.0,
    ]
}

impl Render for BattleView {
    fn render(&mut self, _window: &mut Window, _cx: &mut Context<Self>) -> impl IntoElement {
        let viewport = _window.viewport_size();
        let space = GameState::compute_layout(f32::from(viewport.width), f32::from(viewport.height));
        let piece_size = (space * 0.7).floor();
        let center_offset = (space - piece_size) / 2.0;

        // The board container includes a 1px ring for the border, matching the
        // C++ DrawRectangleLines drawn at (x_start - 1, y_start - 1).
        let board_size = space * BOARD_DIM as f32 + BORDER_WIDTH * 2.0;
        let cells_offset = BORDER_WIDTH;

        let mut piece_cells = Vec::new();
        for y in 0..BOARD_DIM {
            for x in 0..BOARD_DIM {
                let piece = self.game.board[x + y * BOARD_DIM];
                let Some(kind) = piece_kind_byte(piece.kind) else {
                    continue;
                };
                // Always the white sprite; black is tinted at paint time (C++).
                let Some(sprite) = sprite(0, kind) else {
                    continue;
                };
                let tint = match piece.color {
                    PieceColor::White => NORD4,
                    PieceColor::Black => NORD3,
                    PieceColor::None => continue,
                };
                piece_cells.push((x, y, sprite, tint));
            }
        }

        let server_uri = self.server_uri.clone();
        let move_count = self.game.history.len();
        let turn_label = match self.game.turn {
            PieceColor::White => "White",
            PieceColor::Black => "Black",
            PieceColor::None => "-",
        };

        let light_fill = nord::hex(NORD1);
        let dark_fill = nord::hex(NORD_BACKGROUND);
        let border_color = nord::hex(NORD_FOREGROUND);
        let overlay_color = nord::hex(NORD_PRIMARY);

        div()
            .relative()
            .size_full()
            .flex()
            .flex_col()
            .items_center()
            .justify_center()
            .bg(nord::hex(NORD_BACKGROUND))
            .child(
                div()
                    .relative()
                    .w(px(board_size))
                    .h(px(board_size))
                    .child(
                        canvas(
                            move |_, _, _| {},
                            move |bounds, _, window, _| {
                                let origin = bounds.origin;
                                for y in 0..BOARD_DIM {
                                    for x in 0..BOARD_DIM {
                                        let cell = Bounds::new(
                                            point(
                                                origin.x + px(cells_offset + x as f32 * space),
                                                origin.y + px(cells_offset + y as f32 * space),
                                            ),
                                            size(px(space), px(space)),
                                        );
                                        let fill = if (x + y) % 2 == 0 {
                                            light_fill
                                        } else {
                                            dark_fill
                                        };
                                        window.paint_quad(quad(
                                            cell,
                                            px(0.),
                                            fill,
                                            px(0.),
                                            transparent_black(),
                                            BorderStyle::Solid,
                                        ));
                                    }
                                }

                                // Per-pixel piece sprites: nearest-neighbor equivalent via
                                // one solid quad per opaque texel (gpui img() is bilinear).
                                // Source is always white art; tint matches C++ DrawTexturePro.
                                for &(x, y, sprite, tint) in &piece_cells {
                                    let base_x =
                                        cells_offset + x as f32 * space + center_offset;
                                    let base_y =
                                        cells_offset + y as f32 * space + center_offset;
                                    let scale_x = piece_size / sprite.width as f32;
                                    let scale_y = piece_size / sprite.height as f32;

                                    for sy in 0..sprite.height {
                                        for sx in 0..sprite.width {
                                            let idx =
                                                ((sy * sprite.width + sx) * 4) as usize;
                                            let a = sprite.rgba[idx + 3];
                                            if a == 0 {
                                                continue;
                                            }
                                            let [r, g, b] = tinted_rgb(
                                                [
                                                    sprite.rgba[idx],
                                                    sprite.rgba[idx + 1],
                                                    sprite.rgba[idx + 2],
                                                ],
                                                tint,
                                            );
                                            let color = Rgba {
                                                r,
                                                g,
                                                b,
                                                a: a as f32 / 255.0,
                                            };
                                            let texel = Bounds::new(
                                                point(
                                                    origin.x
                                                        + px(base_x + sx as f32 * scale_x),
                                                    origin.y
                                                        + px(base_y + sy as f32 * scale_y),
                                                ),
                                                size(px(scale_x), px(scale_y)),
                                            );
                                            window.paint_quad(quad(
                                                texel,
                                                px(0.),
                                                color,
                                                px(0.),
                                                transparent_black(),
                                                BorderStyle::Solid,
                                            ));
                                        }
                                    }
                                }

                                window.paint_quad(quad(
                                    bounds,
                                    px(0.),
                                    transparent_black(),
                                    px(BORDER_WIDTH),
                                    border_color,
                                    BorderStyle::Solid,
                                ));
                            },
                        )
                        .size_full(),
                    ),
            )
            .child(
                div()
                    .absolute()
                    .left(px(OVERLAY_INSET))
                    .top(px(OVERLAY_INSET))
                    .flex()
                    .flex_col()
                    .gap_1()
                    .text_sm()
                    .font_family(FONT_FAMILY)
                    .text_color(overlay_color)
                    .child(format!("Server: {server_uri}"))
                    .child(format!("Moves: {move_count}"))
                    .child(format!("Turn: {turn_label}")),
            )
    }
}
