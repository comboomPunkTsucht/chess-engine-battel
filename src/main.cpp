// #include "assets.h"
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

enum piece_t {
  none,
  pawn,
  rook,
  horse,
  bishop,
  queen,
  king,
};

enum piece_color {
  is_white,
  is_black,
};

typedef struct {
  piece_t type;
  int step_counter;
  piece_color color;
} piece;

piece game_state[8][8] = { (piece){none, 0, is_white} };

void interact(void) {}
void render(void) {
  int space_size = (std::min(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT) * 0.9) / DEFAULT_BOARD_DIMENSIONS;
  int x_start = (DEFAULT_SCREEN_WIDTH - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  int y_start = (DEFAULT_SCREEN_HEIGHT - (space_size * DEFAULT_BOARD_DIMENSIONS)) / 2;
  
  BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(1, 1);

    for (int x = 0; x < DEFAULT_BOARD_DIMENSIONS; x++) {
      for (int y = 0; y < DEFAULT_BOARD_DIMENSIONS; y++) {
        DrawRectangleLines(x * space_size + x_start, y * space_size + y_start, space_size, space_size, GRAY);
        
        switch (game_state[x][y].type) {
          case none:
            break;
          case pawn:
            DrawText("p", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          case rook:
            DrawText("r", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          case horse:
            DrawText("h", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          case bishop:
            DrawText("b", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          case queen:
            DrawText("q", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          case king:
            DrawText("k", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, GREEN);
            break;
          default:
            DrawText("error", x * space_size + x_start, y * space_size + y_start + 4, space_size / 2, RED);
            break;
        }
      }
    }

  EndDrawing();
}
void update(void) {}


void game(void) {
  while (!WindowShouldClose()) {
    interact();
    update();
    render();
  }
}

int main(void) {
  addon_init_logging();

  float font_size = std::max(floor(DEFAULT_SCREEN_WIDTH * 0.044 + 0.2), floor(DEFAULT_SCREEN_HEIGHT * 0.025));

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT |
    FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | /* FLAG_WINDOW_RESIZABLE | */
    FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Raylib Bouncing Circle");


  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
  
  game_state[3][4].type = bishop;

  game();

  CloseWindow();

  return 0;
}
