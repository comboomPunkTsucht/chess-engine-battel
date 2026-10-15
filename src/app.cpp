#include "app.h"
#include "global.h"
#include "raylib.h"
#include <cmath>

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

	SetWindowIcon(LoadImage(temp_sprintf("%s/logo.png", assetPath)));

#endif
	chess_piece_textures = LoadTexture(temp_sprintf("%s/piece/All_pieces.png", assetPath));
	App::load_font();
}

void App::load_font() {
	font_size = getFontSize();

	main_font = LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Regular.ttf", assetPath), font_size, NULL, 0);
	bold_font = LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Bold.ttf", assetPath), font_size, NULL, 0);
	italic_font = LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Italic.ttf", assetPath), font_size, NULL, 0);
}

void App::startGame() {
	delete mCurrentGame;
	mCurrentGame = new Game();
	mCurrentGame->setup_board();
}
