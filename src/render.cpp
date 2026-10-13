#include "game.h"
#include "raylib.h"
#include <algorithm>
#include <math.h>

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

void Game::render(Game *game) {
  const int space_size =
      (std::min(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) * 0.9) /
      DEFAULT_BOARD_DIMENSIONS;
  const int x_start =
      (DEFAULT_SCREEN_WIDTH - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  const int y_start =
      (DEFAULT_SCREEN_HEIGHT - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;

  BeginDrawing();
  ClearBackground(NORD_BACKGROUND_COLOR);
  DrawFPS(1, 1);

  for (uint32_t x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
    for (uint32_t y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
      int px = x * space_size + x_start;
      int py = y * space_size + y_start;

      DrawRectangleLines(px, py, space_size, space_size, GRAY);

      uint32_t idx = x + y * DEFAULT_BOARD_DIMENSIONS;
      Piece    piece = game->board[idx];

      switch (piece.type) {
        case None: break;
        case Pawn:
          DrawText("p", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case Rook:
          DrawText("r", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case Knight:
          DrawText("kn", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case Bishop:
          DrawText("b", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case Queen:
          DrawText("q", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case King:
          DrawText("kg", px + 4, py + 4, space_size / 2, NORD_PRIMARY_COLOR);
          break;
        default: DrawText("error", px + 4, py + 4, space_size / 2, RED); break;
      }
    }
  }

  EndDrawing();
}
