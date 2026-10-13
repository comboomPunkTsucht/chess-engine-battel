#include "game.h"
#include "raylib.h"
#include "nob_addons.h"

void Game::render(Game* game) {
  const int space_size = (std::min(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) * 0.9) / DEFAULT_BOARD_DIMENSIONS;
  const int x_start = (DEFAULT_SCREEN_WIDTH - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  const int y_start = (DEFAULT_SCREEN_HEIGHT - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  
  BeginDrawing();
  ClearBackground(NORD_BACKGROUND_COLOR);
  DrawFPS(1, 1);

    for (int x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
      for (int y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
        DrawRectangleLines(x * space_size + x_start, y * space_size + y_start, space_size, space_size, GRAY);
        
        switch (game_state[x][y].type) {
          case None:
            break;
          case Pawn:
            DrawText("p", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          case Rook:
            DrawText("r", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          case Knight:
            DrawText("h", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          case Bishop:
            DrawText("b", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          case Queen:
            DrawText("q", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          case King:
            DrawText("k", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, NORD_PRIMARY_COLOR);
            break;
          default:
            DrawText("error", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, RED);
            break;
        }
      }
    }

  EndDrawing();
}