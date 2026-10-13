#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "flag.h"
#include "move.h"
#include "game.h"

// #include <ixwebsocket/IXConnectionState.h>
// #include <ixwebsocket/IXWebSocket.h>
// #include <ixwebsocket/IXWebSocketServer.h>

int main(void) {
  const float font_size = std::max(floor(DEFAULT_SCREEN_WIDTH * 0.044 + 0.2), floor(DEFAULT_SCREEN_HEIGHT * 0.025));

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT |
                 FLAG_WINDOW_HIGHDPI |
                 FLAG_VSYNC_HINT | /* FLAG_WINDOW_RESIZABLE | */
                 FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
             "Chess Engine Battle");
  Game game = Game();
  game.setup_board();
  game.main_loop();

  CloseWindow();

  return 0;
}
