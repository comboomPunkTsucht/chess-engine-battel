#![allow(dead_code)]

use std::fmt;
use std::str::FromStr;

use crate::piece::PieceType;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum MoveType {
    Normal = 0,
    DoublePush = 1,
    Capture = 2,
    EnPassant = 3,
    CastleKing = 4,
    CastleQueen = 5,
    PromoKnight = 6,
    PromoBishop = 7,
    PromoRook = 8,
    PromoQueen = 9,
    PromoCaptureKnight = 10,
    PromoCaptureBishop = 11,
    PromoCaptureRook = 12,
    PromoCaptureQueen = 13,
}

impl MoveType {
    pub fn from_u8(value: u8) -> Self {
        match value {
            0 => MoveType::Normal,
            1 => MoveType::DoublePush,
            2 => MoveType::Capture,
            3 => MoveType::EnPassant,
            4 => MoveType::CastleKing,
            5 => MoveType::CastleQueen,
            6 => MoveType::PromoKnight,
            7 => MoveType::PromoBishop,
            8 => MoveType::PromoRook,
            9 => MoveType::PromoQueen,
            10 => MoveType::PromoCaptureKnight,
            11 => MoveType::PromoCaptureBishop,
            12 => MoveType::PromoCaptureRook,
            13 => MoveType::PromoCaptureQueen,
            _ => MoveType::Normal,
        }
    }

    pub fn as_u8(self) -> u8 {
        self as u8
    }
}

/// 32-bit packed move.
///
/// Bit layout:
/// - 0-5   from square (0..63)
/// - 6-11  to square (0..63)
/// - 12-15 move type
/// - 16-19 captured piece type
/// - 20-23 promotion piece type
/// - 24-27 previous castling rights
/// - 28-31 previous en passant file
#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct Move {
    pub data: u32,
}

#[derive(Debug, Clone, PartialEq, Eq)]
pub struct ParseMoveError {
    pub raw: String,
}

impl fmt::Display for ParseMoveError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "invalid move payload {:?}", self.raw)
    }
}

impl std::error::Error for ParseMoveError {}

impl Move {
    pub const NONE: Move = Move { data: 0 };

    pub fn new(data: u32) -> Self {
        Self { data }
    }

    pub fn none() -> Self {
        Self::NONE
    }

    pub fn is_none(self) -> bool {
        self.data == 0
    }

    pub fn from(self) -> usize {
        (self.data & 0x3F) as usize
    }

    pub fn to(self) -> usize {
        ((self.data >> 6) & 0x3F) as usize
    }

    pub fn move_type(self) -> MoveType {
        MoveType::from_u8(((self.data >> 12) & 0xF) as u8)
    }

    pub fn captured(self) -> PieceType {
        PieceType::from_u8(((self.data >> 16) & 0xF) as u8)
    }

    pub fn promo(self) -> PieceType {
        PieceType::from_u8(((self.data >> 20) & 0xF) as u8)
    }

    pub fn prev_castling(self) -> u8 {
        ((self.data >> 24) & 0xF) as u8
    }

    pub fn prev_en_passant_file(self) -> u8 {
        ((self.data >> 28) & 0xF) as u8
    }

    pub fn is_capture(self) -> bool {
        self.captured() != PieceType::None || self.move_type() == MoveType::EnPassant
    }

    pub fn is_promotion(self) -> bool {
        self.promo() != PieceType::None
    }

    pub fn is_castle(self) -> bool {
        matches!(self.move_type(), MoveType::CastleKing | MoveType::CastleQueen)
    }

    #[allow(clippy::too_many_arguments)]
    pub fn make_move(
        from: usize,
        to: usize,
        move_type: MoveType,
        captured: PieceType,
        promo: PieceType,
        prev_castling: u8,
        prev_en_passant_file: u8,
    ) -> Self {
        debug_assert!(from < 64);
        debug_assert!(to < 64);
        Self {
            data: (from as u32 & 0x3F)
                | ((to as u32 & 0x3F) << 6)
                | ((move_type as u32) << 12)
                | ((captured as u32) << 16)
                | ((promo as u32) << 20)
                | (((prev_castling & 0xF) as u32) << 24)
                | (((prev_en_passant_file & 0xF) as u32) << 28),
        }
    }

    pub fn quiet(from: usize, to: usize) -> Self {
        Self::make_move(from, to, MoveType::Normal, PieceType::None, PieceType::None, 0, 0)
    }

    pub fn double_push(from: usize, to: usize) -> Self {
        Self::make_move(from, to, MoveType::DoublePush, PieceType::None, PieceType::None, 0, 0)
    }

    pub fn capture(from: usize, to: usize, captured: PieceType) -> Self {
        Self::make_move(from, to, MoveType::Capture, captured, PieceType::None, 0, 0)
    }

    pub fn en_passant(from: usize, to: usize, captured_pawn: PieceType) -> Self {
        Self::make_move(from, to, MoveType::EnPassant, captured_pawn, PieceType::None, 0, 0)
    }

    pub fn castle_king(from: usize, to: usize) -> Self {
        Self::make_move(from, to, MoveType::CastleKing, PieceType::None, PieceType::None, 0, 0)
    }

    pub fn castle_queen(from: usize, to: usize) -> Self {
        Self::make_move(from, to, MoveType::CastleQueen, PieceType::None, PieceType::None, 0, 0)
    }

    pub fn promotion(from: usize, to: usize, promo: PieceType, captured: PieceType) -> Self {
        let move_type = match (promo, captured) {
            (PieceType::Knight, PieceType::None) => MoveType::PromoKnight,
            (PieceType::Knight, _) => MoveType::PromoCaptureKnight,
            (PieceType::Bishop, PieceType::None) => MoveType::PromoBishop,
            (PieceType::Bishop, _) => MoveType::PromoCaptureBishop,
            (PieceType::Rook, PieceType::None) => MoveType::PromoRook,
            (PieceType::Rook, _) => MoveType::PromoCaptureRook,
            (PieceType::Queen, PieceType::None) => MoveType::PromoQueen,
            (PieceType::Queen, _) => MoveType::PromoCaptureQueen,
            _ => MoveType::Normal,
        };
        Self::make_move(from, to, move_type, captured, promo, 0, 0)
    }
}

impl FromStr for Move {
    type Err = ParseMoveError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        s.trim()
            .parse::<u32>()
            .map(Move::new)
            .map_err(|_| ParseMoveError { raw: s.to_owned() })
    }
}

impl fmt::Display for Move {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.data)
    }
}
