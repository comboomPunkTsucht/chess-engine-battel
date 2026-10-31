#include "app.h"

#include "nob_addons.h"
#include "raylib.h"
#include "wsConnection.h"
#include <cmath>
#include <cstring>

#define HT_IMPLEMENTATION
#include "ht.h"

typedef struct {
	float size;
	App::FontType type;
} HT_Font;
static Ht(HT_Font, Font) ht_Fonts = {0};
// Baut den Pfad relativ zur ausführbaren Datei zusammen
const char *App::appDir = strdup(GetApplicationDirectory());

#ifdef __APPLE__
const char *App::assetPath = strdup(TextFormat("%s/../Resources/", appDir));
#else
const char *App::assetPath = strdup(TextFormat("%s/assets/", appDir));
#endif

Texture2D App::chess_piece_textures;

size_t App::temp_mark;

const float App::font_size_factor = 0.015625f;

int App::screen_width = DEFAULT_SCREEN_WIDTH;
int App::screen_height = DEFAULT_SCREEN_HEIGHT;

int App::targetFPS = GetMonitorRefreshRate(GetCurrentMonitor());
bool App::vsync_enabled = true;

const char *App::server_address = "0.0.0.0";
uint64_t App::server_port = 14318;
const char *App::server_uri = nullptr;

float App::getFontSize(float size_factor = 20.0f) {
	return floor((std::max(std::floor(App::screen_width * App::font_size_factor + 0.00015625f), std::floor(App::screen_height * 0.0277f)) *
	              (size_factor * (10.0f / 3.0f))) *
	             App::font_size_factor);
}

void App::resize_window() {

	temp_rewind(temp_mark);
	if (IsWindowResized() || App::screen_width != GetScreenWidth() || App::screen_height != GetScreenHeight()) {
		App::screen_width = GetScreenWidth();
		App::screen_height = GetScreenHeight();

		TraceLog(LOG_INFO, "Window resized to: %d x %d", App::screen_width, App::screen_height);

		if (App::screen_width < MIN_SCREEN_WIDTH) {
			App::screen_width = MIN_SCREEN_WIDTH;
			SetWindowSize(MIN_SCREEN_WIDTH, App::screen_height);
			TraceLog(LOG_INFO, "Window size adjusted to: %d x %d", App::screen_width, App::screen_height);
		}
		if (App::screen_height < MIN_SCREEN_HEIGHT) {
			App::screen_height = MIN_SCREEN_HEIGHT;
			SetWindowSize(App::screen_width, MIN_SCREEN_HEIGHT);
			TraceLog(LOG_INFO, "Window size adjusted to: %d x %d", App::screen_width, App::screen_height);
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

	App::targetFPS = GetMonitorRefreshRate(GetCurrentMonitor());

	SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_INTERLACED_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, "Chess Engine Battle");

	SetWindowMinSize(MIN_SCREEN_WIDTH, MIN_SCREEN_HEIGHT);

	SetTargetFPS(App::targetFPS);

#if defined(_WIN32) || (defined(__linux__) && defined(USE_X11))

	SetWindowIcon(LoadImage(TextFormat("%s/logo.png", App::assetPath)));

#endif
	App::chess_piece_textures = LoadTexture(TextFormat("%s/piece/All_pieces.png", App::assetPath));

	setServerUri();

	wsConnection = new WSConnection(this);
}

Font App::get_font(FontType type, float font_size) {
	if (ht_find(&ht_Fonts, (HT_Font){getFontSize(font_size), type}) == NULL) {
		HT_Font new_font = {getFontSize(font_size), type};
		switch (type) {
			case FONT_REGULAR:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Regular.ttf", App::assetPath), getFontSize(font_size), NULL, 0);
				break;
			case FONT_BOLD:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Bold.ttf", App::assetPath), getFontSize(font_size), NULL, 0);
				break;
			case FONT_ITALIC:
				*ht_put(&ht_Fonts, new_font) =
				    LoadFontEx(temp_sprintf("%s/CaskaydiaCoveNerdFontPropo-Italic.ttf", App::assetPath), getFontSize(font_size), NULL, 0);
				break;
		}
	}
	return *ht_find(&ht_Fonts, (HT_Font){getFontSize(font_size), type});
}

void App::startGame() {
	wsConnection->startServer();
	delete mCurrentGame;
	mCurrentGame = new Game();
	mCurrentGame->setup_board();
}

void restartWebsocketServer() {
	wsConnection->stopServer();
	wsConnection->startServer();
}

void App::setWebsocketServerAddress(const char *address = "0.0.0.0") {
	server_address = strdup(address);
	setServerUri();
}
void App::setWebsocketServerPort(uint16_t port = 14318) {
	server_port = port;
	setServerUri();
}

void App::setServerUri() {
	if (server_uri != nullptr) { free((void *)server_uri); }
	server_uri = strdup(TextFormat("ws://%s:%d", server_address, server_port));
	nob_log(INFO, "Websocket server URI: %s", server_uri);
}
