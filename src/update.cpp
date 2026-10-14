#include "game.h"
#include "global.h"
#include "nob_addons.h"
#include <cmath>

void Game::update() {

  if (IsWindowResized()) {
    screen_width = GetScreenWidth();
    screen_height = GetScreenHeight();
  }

  float font_size = std::max(std::floor(screen_width * font_size_factor + 0.2),
                             std::floor(screen_height * 0.025));

  main_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Regular_ttf_size, font_size, NULL, 0);
  bold_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Bold_ttf_size, font_size, NULL, 0);
  italic_font = LoadFontFromMemory(
      ".ttf", Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf,
      Assets::CaskaydiaCoveNerdFontPropo_Italic_ttf_size, font_size, NULL, 0);
}
