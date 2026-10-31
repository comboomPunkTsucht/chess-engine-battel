#include "app.h"
#include "global.h"
#include "nob_addons.h"
#include "raylib.h"
#include "wsConnection.h"
#include <cstring>

#define HT_IMPLEMENTATION
#include "ht.h"

typedef struct {
	float size;
	App::FontType type;
} HT_Font;
Ht(HT_Font, Font) ht_Fonts = {0};

const float App::font_size_factor = 0.015625f;

float App::getFontSize(float size_factor = 20.0f) {
	return floor((std::max(std::floor(screen_width * App::font_size_factor + 0.00015625f), std::floor(screen_height * 0.0277f)) *
	              (size_factor * (10.0f / 3.0f))) *
	             App::font_size_factor);
}

void App::resize_window() {

	temp_rewind(temp_mark);
	if (IsWindowResized()) {
		screen_width = GetScreenWidth();
		screen_height = GetScreenHeight();

		TraceLog(LOG_INFO, "Window resized to: %d x %d", screen_width, screen_height);

		if (screen_width < MIN_SCREEN_WIDTH) {
			screen_width = MIN_SCREEN_WIDTH;
			SetWindowSize(MIN_SCREEN_WIDTH, screen_height);
			TraceLog(LOG_INFO, "Window size adjusted to: %d x %d", screen_width, screen_height);
		}
		if (screen_height < MIN_SCREEN_HEIGHT) {
			screen_height = MIN_SCREEN_HEIGHT;
			SetWindowSize(screen_width, MIN_SCREEN_HEIGHT);
			TraceLog(LOG_INFO, "Window size adjusted to: %d x %d", screen_width, screen_height);
		}
	}
}

void App::main_loop() {
	while (!WindowShouldClose()) {
		resize_window();
		processInput();
		update();
		render();
	}
}

App::~App() {
	wsConnection->stopServer();
	delete wsConnection;
}

App::App() {

	server_uri = setServerUri();

	wsConnection = new WSConnection();

	wsConnection->startServer();

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT | FLAG_WINDOW_HIGHDPI | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Chess Engine Battle");

	SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);

	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

#if defined(_WIN32) || (defined(__linux__) && defined(USE_X11))

	SetWindowIcon(LoadImage(TextFormat("%s/logo.png", assetPath)));

#endif
	chess_piece_textures = LoadTexture(TextFormat("%s/piece/All_pieces.png", assetPath));
}

Font App::get_font(FontType type, float font_size) {
	if (ht_find(&ht_Fonts, (HT_Font){getFontSize(font_size), type}) == NULL) {
		HT_Font new_font = {getFontSize(font_size), type};
		switch (type) {
			case FONT_REGULAR:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Regular.ttf", assetPath), getFontSize(font_size), NULL, 0);
				break;
			case FONT_BOLD:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Bold.ttf", assetPath), getFontSize(font_size), NULL, 0);
				break;
			case FONT_ITALIC:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Italic.ttf", assetPath), getFontSize(font_size), NULL, 0);
				break;
		}
	}
	return *ht_find(&ht_Fonts, (HT_Font){getFontSize(font_size), type});
}

void App::startGame() {
	delete mCurrentGame;
	mCurrentGame = new Game();
	mCurrentGame->setup_board();
}

void App::setWebsocketServerAddress(const char *address = "0.0.0.0") {
	strcpy(server_address, address);
	setServerUri();
}
void App::setWebsocketServerPort(uint16_t port = 14318) {
	server_port = port;
	setServerUri();
}
