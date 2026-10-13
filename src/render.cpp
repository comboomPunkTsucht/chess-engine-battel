#include "game.h"
#include "nord.h"
#include "piece.h"
#include "raylib.h"
#include <algorithm>
#include <math.h>

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
      float px = x * space_size + x_start;
      float py = y * space_size + y_start;

      DrawRectangleLines(px, py, space_size, space_size, NORD_FOREGROUND_COLOR);

      uint32_t idx = x + y * DEFAULT_BOARD_DIMENSIONS;
      Piece    piece = game->board[idx];

      switch (piece.type) {
        case None: break;
        default:
          if (piece.color == PLAYER_WHITE) {
            DrawTextureRec(
                game->pieces,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 0.0,
                            20.0, 19.0},
                (Vector2){px, py}, NORD_HIGHLIGHT_COLOR);
          }
          if (piece.color == PLAYER_BLACK) {
            DrawTextureRec(
                game->pieces,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 19.0,
                            20.0, 19.0},
                (Vector2){px, py}, NORD_PRIMARY_COLOR);
          }

          break;
      }
      Rectangle hello;
    }
  }
  // DrawTexture(game->pieces, 1, 1, NORD_FOREGROUND_COLOR);
  EndDrawing();
}
