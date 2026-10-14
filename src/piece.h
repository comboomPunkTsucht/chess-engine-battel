#pragma once
#include "assets.h"
#include "raylib.h"
#include <cstdint>

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

static Image chess_piece_image = LoadImageFromMemory(
    ".png", Assets::All_pieces_png, Assets::All_pieces_png_size);

static Texture2D chess_piece_textures = LoadTextureFromImage(chess_piece_image);
