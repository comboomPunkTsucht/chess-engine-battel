// #include "assets.h"
#include "move.h"
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// NEU: IXWebSocket Includes
#include <ixwebsocket/IXConnectionState.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>

#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

#include "raygui.h"

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

// #define HT_IMPLEMENTATION
// #include "ht.h"

#define DEFAULT_SCREEN_WIDTH 800
#define DEFAULT_SCREEN_HEIGHT 450
#define DEFAULT_BOARD_DIMENSIONS 8
#define FONT_SIZE 48

enum piece_color {
  CWHITE,
  CBLACK,
};

typedef struct {
    PieceType   type;
    int         step_counter;
    piece_color color;
} piece;

piece game_state[DEFAULT_BOARD_DIMENSIONS][DEFAULT_BOARD_DIMENSIONS] = {
    (piece){PieceType::None, 0, CWHITE}};

void load(void) { SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor())); }

void interact(void) { }
void render(void) {
  int space_size =
      (std::min(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) * 0.9) /
      DEFAULT_BOARD_DIMENSIONS;
  int x_start =
      (DEFAULT_SCREEN_WIDTH - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  int y_start =
      (DEFAULT_SCREEN_HEIGHT - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;

  float  font_size = std::max(floor(DEFAULT_SCREEN_WIDTH * 0.044 + 0.2),
                              floor(DEFAULT_SCREEN_HEIGHT * 0.025));

  size_t temp_mark = temp_save();

  BeginDrawing();
  temp_rewind(temp_mark);
  ClearBackground(NORD_BACKGROUND_COLOR);
  DrawText(temp_sprintf("%d FPS", GetFPS()), 10, 10, font_size,
           NORD_PRIMARY_COLOR);

  for (int x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
    for (int y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
      DrawRectangleLines(x * space_size + x_start, y * space_size + y_start,
                         space_size, space_size, GRAY);

      switch (game_state[x][y].type) {
        case PieceType::None: break;
        case PieceType::Pawn:
          DrawText("p", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case PieceType::Rook:
          DrawText("r", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case PieceType::Knight:
          DrawText("h", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case PieceType::Bishop:
          DrawText("b", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case PieceType::Queen:
          DrawText("q", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        case PieceType::King:
          DrawText("k", x * space_size + x_start, y * space_size + y_start + 4,
                   space_size / 2, NORD_PRIMARY_COLOR);
          break;
        default:
          DrawText("error", x * space_size + x_start,
                   y * space_size + y_start + 4, space_size / 2,
                   NORD_ERROR_COLOR);
          break;
      }
    }
  }

  EndDrawing();
}
void update(void) { }

void game(void) {
  while (!WindowShouldClose()) {
    interact();
    update();
    render();
  }
}

int main(void) {
  addon_init_logging();

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT |
                 FLAG_WINDOW_HIGHDPI |
                 FLAG_VSYNC_HINT | /* FLAG_WINDOW_RESIZABLE | */
                 FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
             "Chess Engine Battle");

  load();

  game_state[3][4].type = PieceType::Bishop;

  game();

  CloseWindow();

  return 0;
}
