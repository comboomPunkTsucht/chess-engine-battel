use bevy::prelude::Resource;

use crate::moves::{Move, MoveType};
use crate::piece::{Piece, PieceColor, PieceType};

pub const BOARD_DIM: usize = 8;

#[derive(Resource, Debug, Clone)]
pub struct GameState {
    pub board: [Piece; BOARD_DIM * BOARD_DIM],
    pub history: Vec<Move>,
    pub turn: PieceColor,
    pub white_castle_king_side: bool,
    pub white_castle_queen_side: bool,
    pub black_castle_king_side: bool,
    pub black_castle_queen_side: bool,
    pub fifty_move_counter: i32,
}

impl Default for GameState {
    fn default() -> Self {
        Self::new()
    }
}

impl GameState {
    pub fn new() -> Self {
        let mut game = Self {
            board: [Piece::EMPTY; BOARD_DIM * BOARD_DIM],
            history: Vec::with_capacity(128),
            turn: PieceColor::White,
            white_castle_king_side: true,
            white_castle_queen_side: true,
            black_castle_king_side: true,
            black_castle_queen_side: true,
            fifty_move_counter: 0,
        };
        game.setup_board();
        game
    }

    pub fn setup_board(&mut self) {
        let back_rank = [
            PieceType::Rook,
            PieceType::Knight,
            PieceType::Bishop,
            PieceType::Queen,
            PieceType::King,
            PieceType::Bishop,
            PieceType::Knight,
            PieceType::Rook,
        ];

        for (file, kind) in back_rank.iter().enumerate() {
            self.board[file] = Piece::new(*kind, PieceColor::White);
            self.board[56 + file] = Piece::new(*kind, PieceColor::Black);
        }

        for file in 0..BOARD_DIM {
            self.board[8 + file] = Piece::new(PieceType::Pawn, PieceColor::White);
            self.board[48 + file] = Piece::new(PieceType::Pawn, PieceColor::Black);
        }

        for index in 16..48 {
            self.board[index] = Piece::empty();
        }
    }

    pub fn do_move(&mut self, movement: Move) {
        let from = movement.from();
        let to = movement.to();

        let mut moving = self.board[from];
        self.board[from] = Piece::empty();

        if movement.move_type() == MoveType::EnPassant {
            let captured_pawn_idx = if moving.color == PieceColor::White {
                to.wrapping_sub(BOARD_DIM)
            } else {
                to + BOARD_DIM
            };
            if captured_pawn_idx < self.board.len() {
                self.board[captured_pawn_idx] = Piece::empty();
            }
        }

        match movement.move_type() {
            MoveType::CastleKing => {
                let (rook_from, rook_to) = if moving.color == PieceColor::White {
                    (7, 5)
                } else {
                    (63, 61)
                };
                self.board[rook_to] = self.board[rook_from];
                self.board[rook_from] = Piece::empty();
            }
            MoveType::CastleQueen => {
                let (rook_from, rook_to) = if moving.color == PieceColor::White {
                    (0, 3)
                } else {
                    (56, 59)
                };
                self.board[rook_to] = self.board[rook_from];
                self.board[rook_from] = Piece::empty();
            }
            _ => {}
        }

        if movement.is_castle() {
            if self.turn == PieceColor::White {
                self.white_castle_king_side = false;
                self.white_castle_queen_side = false;
            } else if self.turn == PieceColor::Black {
                self.black_castle_king_side = false;
                self.black_castle_queen_side = false;
            }
        }

        if matches!(
            movement.move_type(),
            MoveType::Normal | MoveType::CastleKing | MoveType::CastleQueen
        ) {
            self.fifty_move_counter += 1;
        } else {
            self.fifty_move_counter = 0;
        }

        if movement.promo() != PieceType::None {
            moving.kind = movement.promo();
        }

        self.board[to] = moving;
        self.history.push(movement);

        self.turn = if self.turn == PieceColor::White {
            PieceColor::Black
        } else {
            PieceColor::White
        };
    }

    /// Mirrors the C++ `Game::computeLayout`: the largest whole-pixel square
    /// size that keeps the board inside 90% of the smaller viewport side.
    pub fn compute_layout(width: f32, height: f32) -> f32 {
        ((width.min(height) * 0.9) / BOARD_DIM as f32).trunc()
    }
}
