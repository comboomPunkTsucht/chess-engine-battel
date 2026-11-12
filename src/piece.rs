#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
#[repr(u8)]
pub enum PieceType {
    #[default]
    None = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6,
}

impl PieceType {
    pub fn from_u8(value: u8) -> Self {
        match value {
            1 => PieceType::Pawn,
            2 => PieceType::Knight,
            3 => PieceType::Bishop,
            4 => PieceType::Rook,
            5 => PieceType::Queen,
            6 => PieceType::King,
            _ => PieceType::None,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub enum PieceColor {
    White,
    Black,
    #[default]
    None,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Default)]
pub struct Piece {
    pub kind: PieceType,
    pub color: PieceColor,
}

impl Piece {
    pub const EMPTY: Piece = Piece {
        kind: PieceType::None,
        color: PieceColor::None,
    };

    pub fn empty() -> Self {
        Self::EMPTY
    }

    pub fn new(kind: PieceType, color: PieceColor) -> Self {
        Self { kind, color }
    }
}
