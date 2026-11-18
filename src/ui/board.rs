use gpui_kit::{
    BorderStyle, Context, IntoElement, ParentElement as _, Rgba, Styled as _, Window, canvas, div,
    point, px, quad, size, transparent_black,
};

use crate::game::{BOARD_DIM, GameState};
use crate::nord::{self, FONT_FAMILY, NORD0, NORD1, NORD3, NORD4, NORD6, NORD_BACKGROUND, NORD_FOREGROUND};
use crate::piece::{PieceColor, PieceType};
use crate::ui::BattleView;

include!(concat!(env!("OUT_DIR"), "/piece_sprites.rs"));

const BORDER_WIDTH: f32 = 1.0;

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

pub fn render(view: &BattleView, _cx: &mut Context<BattleView>) -> impl IntoElement {
    let mut piece_cells = Vec::new();
    for y in 0..BOARD_DIM {
        for x in 0..BOARD_DIM {
            let piece = view.game.board[x + y * BOARD_DIM];
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

    let server_label = format!("Server: ws://{}", view.server.addr);
    let move_count = view.game.history.len();
    let turn_label = match view.game.turn {
        PieceColor::White => "White",
        PieceColor::Black => "Black",
        PieceColor::None => "-",
    };

    let light_fill = nord::hex(NORD1);
    let dark_fill = nord::hex(NORD_BACKGROUND);
    let border_color = nord::hex(NORD_FOREGROUND);

    div()
        .size_full()
        .flex()
        .flex_col()
        .child(
            div().flex_1().min_h_0().w_full().relative().child(
                canvas(
                    move |_, _, _| {},
                    move |bounds, _, window, _| {
                        let w = f32::from(bounds.size.width);
                        let h = f32::from(bounds.size.height);
                        let space = GameState::compute_layout(w, h);
                        let piece_size = (space * 0.7).floor();
                        let center_offset = (space - piece_size) / 2.0;

                        // The board container includes a 1px ring for the border,
                        // matching the C++ DrawRectangleLines at (x_start - 1, y_start - 1).
                        let board_size = space * BOARD_DIM as f32 + BORDER_WIDTH * 2.0;
                        let cells_offset = BORDER_WIDTH;
                        let origin_x = bounds.origin.x + px((w - board_size) / 2.0);
                        let origin_y = bounds.origin.y + px((h - board_size) / 2.0);

                        for y in 0..BOARD_DIM {
                            for x in 0..BOARD_DIM {
                                let cell = gpui_kit::Bounds::new(
                                    point(
                                        origin_x + px(cells_offset + x as f32 * space),
                                        origin_y + px(cells_offset + y as f32 * space),
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
                            let base_x = cells_offset + x as f32 * space + center_offset;
                            let base_y = cells_offset + y as f32 * space + center_offset;
                            let scale_x = piece_size / sprite.width as f32;
                            let scale_y = piece_size / sprite.height as f32;

                            for sy in 0..sprite.height {
                                for sx in 0..sprite.width {
                                    let idx = ((sy * sprite.width + sx) * 4) as usize;
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
                                    let texel = gpui_kit::Bounds::new(
                                        point(
                                            origin_x + px(base_x + sx as f32 * scale_x),
                                            origin_y + px(base_y + sy as f32 * scale_y),
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

                        let board_bounds = gpui_kit::Bounds::new(
                            point(origin_x, origin_y),
                            size(px(board_size), px(board_size)),
                        );
                        window.paint_quad(quad(
                            board_bounds,
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
                .w_full()
                .flex()
                .flex_row()
                .justify_between()
                .px(px(12.))
                .py(px(6.))
                .text_sm()
                .font_family(FONT_FAMILY)
                .text_color(nord::hex(NORD6))
                .child(server_label)
                .child(format!("Moves: {move_count}"))
                .child(format!("Turn: {turn_label}")),
        )
        
}
