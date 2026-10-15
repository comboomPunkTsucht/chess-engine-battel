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

	SetWindowIcon(LoadImage(ASSETS_PATH "icon.png"));

#endif
	chess_piece_textures = LoadTexture(ASSETS_PATH "piece/All_pieces.png");
	App::load_font();
}

void App::load_font() {
	font_size = getFontSize();

	main_font = LoadFontEx(ASSETS_PATH "CaskaydiaCoveNerdFontPropo-Regular.ttf", font_size, NULL, 0);
	bold_font = LoadFontEx(ASSETS_PATH "CaskaydiaCoveNerdFontPropo-Bold.ttf", font_size, NULL, 0);
	italic_font = LoadFontEx(ASSETS_PATH "CaskaydiaCoveNerdFontPropo-Italic.ttf", font_size, NULL, 0);
}

void App::startGame() {
	delete mCurrentGame;
	mCurrentGame = new Game();
	mCurrentGame->setup_board();
}
