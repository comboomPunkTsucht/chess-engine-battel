#include "app.h"

#define FLAG_IMPLEMENTATION
#include "flag.h"

#define NOB_IMPLEMENTATION
#include "nob_addons.h"

#include "move.h"
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

// #include <ixwebsocket/IXConnectionState.h>
// #include <ixwebsocket/IXWebSocket.h>
// #include <ixwebsocket/IXWebSocketServer.h>

int main(void) {
  addon_init_logging();

  SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT |
                 FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE |
                 FLAG_WINDOW_ALWAYS_RUN);
  InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT,
             "Chess Engine Battle");
  
  App app = App();
  app.initialization();
  app.startGame();
  app.main_loop();

  CloseWindow();

  return 0;
}
