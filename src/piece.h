#pragma once
#include "global.h"
#include "raylib.h"
#include <cstdint>

enum PieceType : uint8_t { None = 0, Pawn = 1, Knight = 2, Bishop = 3, Rook = 4, Queen = 5, King = 6 };

enum PieceColor { PLAYER_WHITE, PLAYER_BLACK, PLAYER_NONE };

struct Piece {
	PieceType type;
	PieceColor color;
};
