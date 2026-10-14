#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include <cmath>

void Game::update() {

  if (IsWindowResized()) {
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    Game::load_font();
  }
}
