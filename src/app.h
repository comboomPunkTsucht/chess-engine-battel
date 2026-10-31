#pragma once

#include "game.h"
#include <cstdint>

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

	typedef enum { FONT_REGULAR, FONT_BOLD, FONT_ITALIC } FontType;
	static Font get_font(FontType type, float font_size);
	static float getFontSize(float size_factor);

private:
	static const float font_size_factor;

	void resize_window();
};
