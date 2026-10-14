#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include "piece.h"
#include "raylib.h"
#include <algorithm>
#include <math.h>

#include "nord.h"

void Game::render() {
  const int space_size =
      (std::min(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) * 0.9) /
      DEFAULT_BOARD_DIMENSIONS;
  const int x_start =
      (DEFAULT_SCREEN_WIDTH - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  const int y_start =
      (DEFAULT_SCREEN_HEIGHT - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  temp_rewind(temp_mark);
  BeginDrawing();
  ClearBackground(NORD_BACKGROUND_COLOR);
  DrawTextEx(main_font, temp_sprintf("%d FPS", GetFPS()), (Vector2){1, 1}, 20.0,
             2.0, NORD_PRIMARY_COLOR);

  for (uint32_t x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
    for (uint32_t y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
      float px = x * space_size + x_start;
      float py = y * space_size + y_start;

      DrawRectangleLines(px, py, space_size, space_size, NORD_FOREGROUND_COLOR);

      uint32_t idx = x + y * DEFAULT_BOARD_DIMENSIONS;
      Piece    piece = board[idx];

      switch (piece.type) {
        case None: break;
        default:
          if (piece.color == PLAYER_WHITE) {
            DrawTextureRec(
                chess_piece_textures,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 0.0,
                            20.0, 19.0},
                (Vector2){px, py}, NORD_HIGHLIGHT_COLOR);
          }
          if (piece.color == PLAYER_BLACK) {
            DrawTextureRec(
                chess_piece_textures,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 19.0,
                            20.0, 19.0},
                (Vector2){px, py}, NORD_PRIMARY_COLOR);
          }

          break;
      }
    }
  }
  // DrawTexture(game->pieces, 1, 1, NORD_FOREGROUND_COLOR);
  EndDrawing();
}
