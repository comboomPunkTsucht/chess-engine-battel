#include <cmath>
#include "global.h"
#include "app.h"
#include "raylib.h"
#include "assets.h"

void App::main_loop() {
  while (!WindowShouldClose()) {
    processInput();
    update();
    render();
  }
}

void App::initialization(void) {
  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

#ifdef _WIN32

  SetWindowIcon(
      LoadImageFromMemory(".png", Assets::icon_png, Assets::icon_png_size));

#endif

  chess_piece_image = LoadImageFromMemory(".png", Assets::All_pieces_png,
                                          Assets::All_pieces_png_size);
  chess_piece_textures = LoadTextureFromImage(chess_piece_image);

  App::load_font();
}

void App::load_font() {
	font_size = getFontSize();

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

void App::startGame() {
    delete mCurrentGame;
    mCurrentGame = new Game();
    mCurrentGame->setup_board();
}