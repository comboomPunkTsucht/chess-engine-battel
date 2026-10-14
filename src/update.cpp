#include "app.h"
#include "global.h"
#include "nob_addons.h"
#include "raylib.h"
#include <cmath>

void App::update() {

    temp_rewind(temp_mark);
    if (IsWindowResized()) {
        screen_width = GetScreenWidth();
        screen_height = GetScreenHeight();

        App::load_font();
    }
}
