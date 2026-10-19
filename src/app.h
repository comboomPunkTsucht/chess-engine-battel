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

private:
	void load_font();
};
