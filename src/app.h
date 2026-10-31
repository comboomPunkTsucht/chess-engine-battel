#pragma once

#include "game.h"
#include "raylib.h"
#include <cstdint>

#define MIN_SCREEN_WIDTH 1280
#define MIN_SCREEN_HEIGHT 720

#define DEFAULT_SCREEN_WIDTH MIN_SCREEN_WIDTH
#define DEFAULT_SCREEN_HEIGHT MIN_SCREEN_HEIGHT

class App {
public:
	App();
	~App();
	Game *mCurrentGame = nullptr;

	void main_loop();
	void processInput();
	void update();
	void render();

	void startGame();

	void setWebsocketServerAddress(const char *address);
	void setWebsocketServerPort(uint16_t port);
	const char *getWebsocketServerAddress() { return server_address; }
	uint16_t getWebsocketServerPort() { return server_port; }
	const char *getWebsocketServerUri() { return server_uri; }

	typedef enum { FONT_REGULAR, FONT_BOLD, FONT_ITALIC } FontType;
	static Font get_font(FontType type, float font_size);
	static float getFontSize(float size_factor);

	static int targetFPS;
	static bool vsync_enabled;

	static int screen_width;
	static int screen_height;

	static const char *appDir;
	static const char *assetPath;

	static Texture2D chess_piece_textures;

private:
	static const float font_size_factor;

	static const char *server_address;
	static uint64_t server_port;
	static const char *server_uri;

	static void setServerUri();

	static size_t temp_mark;

	void resize_window();
	void processMouseInput();
	void processKeyboardInput();
};
