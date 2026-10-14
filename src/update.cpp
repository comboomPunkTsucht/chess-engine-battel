#include "app.h"
#include "global.h"
#include "nob_addons.h"
#include <cmath>

void App::update() {

  if (IsWindowResized()) {
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();

    App::load_font();
  }
}
