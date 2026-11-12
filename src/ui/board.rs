use std::net::SocketAddr;

use bevy_egui::egui;

use crate::game::{GameState, BOARD_DIM};
use crate::nord::{NORD0, NORD1, NORD3, NORD4, NORD6, hex};
use crate::piece::{PieceColor, PieceType};

include!(concat!(env!("OUT_DIR"), "/piece_sprites.rs"));

/// Tinted piece textures (white art × Nord tint, loaded once per color/kind).
pub struct PieceTextures {
    handles: [[Option<egui::TextureHandle>; 6]; 2],
}

impl Default for PieceTextures {
    fn default() -> Self {
        Self {
            handles: std::array::from_fn(|_| std::array::from_fn(|_| None)),
        }
    }
}

impl PieceTextures {
    pub fn ensure_loaded(&mut self, ctx: &egui::Context) {
        for (color_idx, tint) in [(0usize, NORD4), (1, NORD3)] {
            for kind in 1..=6u8 {
                if self.handles[color_idx][(kind - 1) as usize].is_some() {
                    continue;
                }
                // Source is always the white art; black is only tinted (C++/gpui parity).
                let Some(sprite) = sprite(0, kind) else {
                    continue;
                };
                let handle = load_tinted(ctx, sprite, tint, color_idx as u8, kind);
                self.handles[color_idx][(kind - 1) as usize] = Some(handle);
            }
        }
    }

    pub fn get(&self, color_idx: usize, kind: PieceType) -> Option<&egui::TextureHandle> {
        let kind_idx = (kind as usize).checked_sub(1)?;
        self.handles[color_idx].get(kind_idx)?.as_ref()
    }
}

/// Raylib `DrawTexturePro` tint: per-channel multiply of sprite RGB by the
/// palette color (white pieces → NORD4, black pieces → NORD3).
fn load_tinted(
    ctx: &egui::Context,
    sprite: &Sprite,
    tint: u32,
    color: u8,
    kind: u8,
) -> egui::TextureHandle {
    let [tr, tg, tb, ta] = tint.to_be_bytes();
    let pixels: Vec<egui::Color32> = sprite
        .rgba
        .chunks_exact(4)
        .map(|px| {
            egui::Color32::from_rgba_unmultiplied(
                ((px[0] as u16 * tr as u16) / 255) as u8,
                ((px[1] as u16 * tg as u16) / 255) as u8,
                ((px[2] as u16 * tb as u16) / 255) as u8,
                ((px[3] as u16 * ta as u16) / 255) as u8,
            )
        })
        .collect();
    let image = egui::ColorImage::new([sprite.width as usize, sprite.height as usize], pixels);
    ctx.load_texture(
        format!("piece-{color}-{kind}"),
        image,
        egui::TextureOptions {
            magnification: egui::TextureFilter::Nearest,
            ..Default::default()
        },
    )
}

/// Board painting parity with C++ `render.cpp` and the gpui port: NORD1/NORD0
/// squares, 1px NORD6 border outside, pieces at 0.7×space centered per square,
/// labels for server/moves/turn below the board.
pub fn show_board(
    ui: &mut egui::Ui,
    game: &GameState,
    textures: &PieceTextures,
    server_addr: SocketAddr,
) {
    let (response, painter) = ui.allocate_painter(ui.available_size(), egui::Sense::hover());
    let rect = response.rect;

    painter.rect_filled(rect, 0.0, hex(NORD0));

    let space = GameState::compute_layout(rect.width(), rect.height());
    if space < 1.0 {
        return;
    }

    let board_size = space * BOARD_DIM as f32;
    let x_start = rect.left() + (rect.width() - board_size) / 2.0;
    let y_start = rect.top() + (rect.height() - board_size) / 2.0;
    let board_rect = egui::Rect::from_min_size(
        egui::pos2(x_start, y_start),
        egui::vec2(board_size, board_size),
    );

    // 1px border just outside the board (C++: DrawRectangleLines at -1/+2).
    painter.rect_stroke(
        board_rect,
        0.0,
        egui::Stroke::new(1.0, hex(NORD6)),
        egui::StrokeKind::Outside,
    );

    for y in 0..BOARD_DIM {
        for x in 0..BOARD_DIM {
            let cell = egui::Rect::from_min_size(
                egui::pos2(x_start + x as f32 * space, y_start + y as f32 * space),
                egui::vec2(space, space),
            );
            let fill = if (x + y) % 2 == 0 { NORD1 } else { NORD0 };
            painter.rect_filled(cell, 0.0, hex(fill));
        }
    }

    let piece_size = (space * 0.7).floor();
    let center_offset = (space - piece_size) / 2.0;
    let uv = egui::Rect::from_min_max(egui::pos2(0.0, 0.0), egui::pos2(1.0, 1.0));

    for y in 0..BOARD_DIM {
        for x in 0..BOARD_DIM {
            let piece = game.board[x + y * BOARD_DIM];
            if piece.kind == PieceType::None || piece.color == PieceColor::None {
                continue;
            }
            let color_idx = match piece.color {
                PieceColor::White => 0,
                _ => 1,
            };
            let Some(handle) = textures.get(color_idx, piece.kind) else {
                continue;
            };
            let dest = egui::Rect::from_min_size(
                egui::pos2(
                    x_start + x as f32 * space + center_offset,
                    y_start + y as f32 * space + center_offset,
                ),
                egui::vec2(piece_size, piece_size),
            );
            painter.image(handle.id(), dest, uv, egui::Color32::WHITE);
        }
    }

    let turn = match game.turn {
        PieceColor::White => "White",
        PieceColor::Black => "Black",
        PieceColor::None => "-",
    };
    let font = egui::FontId::proportional(14.0);
    let color = hex(NORD6);
    let label_y = board_rect.bottom() + 6.0;
    painter.text(
        egui::pos2(rect.left() + 6.0, label_y),
        egui::Align2::LEFT_TOP,
        format!("Server: ws://{server_addr}"),
        font.clone(),
        color,
    );
    painter.text(
        egui::pos2(rect.center().x, label_y),
        egui::Align2::CENTER_TOP,
        format!("Moves: {}", game.history.len()),
        font.clone(),
        color,
    );
    painter.text(
        egui::pos2(rect.right() - 6.0, label_y),
        egui::Align2::RIGHT_TOP,
        format!("Turn: {turn}"),
        font,
        color,
    );
}
