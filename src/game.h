#pragma once

#include "move.h"
#include "raylib.h"

struct Piece {
  PieceType type;
  PieceColor color;
  int step_counter;
};

constexpr uint32_t DEFAULT_SCREEN_WIDTH = 800;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 450;
constexpr uint32_t DEFAULT_BOARD_DIMENSIONS = 8;

