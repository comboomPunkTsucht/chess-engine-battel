#pragma once
#include <cstdint>
#include "raylib.h"

enum PieceType : uint8_t {
    None = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

enum PieceColor {
    PLAYER_WHITE,
    PLAYER_BLACK,
};

struct Piece {
    PieceType  type;
    PieceColor color;
};

static Texture2D chess_piece_textures = LoadTexture("assets/piece/All_pieces.png");