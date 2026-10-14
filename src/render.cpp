#include "app.h"
#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include "piece.h"
#include "raylib.h"
#include <algorithm>
#include <cmath>
#include <math.h>

#include "nord.h"

void App::render() {
  mCurrentGame->Game::render();
}

void Game::render() {
  const int space_size = (std::min(screen_width, screen_height) * 0.9) / DEFAULT_BOARD_DIMENSIONS;
  const int x_start = (screen_width - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  const int y_start = (screen_height - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  const float font_size = std::max(std::floor(screen_width * font_size_factor + 0.2), std::floor(screen_height * 0.025));

  temp_rewind(temp_mark);
  BeginDrawing();
  ClearBackground(NORD_BACKGROUND_COLOR);
  DrawTextEx(main_font, temp_sprintf("%d FPS", GetFPS()), (Vector2){1, 1},
             font_size, font_size / 10.0, NORD_PRIMARY_COLOR);

  DrawRectangleLines(
      x_start - 1, y_start - 1, DEFAULT_BOARD_DIMENSIONS * space_size + 2,
      DEFAULT_BOARD_DIMENSIONS * space_size + 2, NORD_FOREGROUND_COLOR);

  for (uint32_t x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
    for (uint32_t y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
      float px = x * space_size + x_start;
      float py = y * space_size + y_start;

      if ((x + y) % 2 == 0) {
        DrawRectangle(px, py, space_size, space_size,
                      get_Nord_Color_as_raylib_color(NORD1));
      }

      uint32_t idx = x + y * DEFAULT_BOARD_DIMENSIONS;
      Piece piece = board[idx];

      switch (piece.type) {
        case None: break;
        default:
          if (piece.color == PLAYER_WHITE) {
            DrawTextureRec(
                chess_piece_textures,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 19.0,
                            20.0, 19.0},
                (Vector2){px, py}, get_Nord_Color_as_raylib_color(NORD3));
          }
          if (piece.color == PLAYER_BLACK) {
            DrawTextureRec(
                chess_piece_textures,
                (Rectangle){(float)(((float)piece.type - 1.0) * 20.0), 19.0,
                            20.0, 19.0},
                (Vector2){px, py}, get_Nord_Color_as_raylib_color(NORD4));
          }

          break;
      }
    }
  }
  // DrawTexture(game->pieces, 1, 1, NORD_FOREGROUND_COLOR);
  EndDrawing();
}
