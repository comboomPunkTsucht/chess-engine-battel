#pragma once

#include <cstdint>
#include "game.h"

constexpr uint32_t DEFAULT_SCREEN_WIDTH = 800;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 450;

inline uint32_t screen_width = DEFAULT_SCREEN_WIDTH;
inline uint32_t screen_height = DEFAULT_SCREEN_HEIGHT;

class App {
public:
    Game* mCurrentGame;

    void main_loop();
    void processInput();
    void update();
    void render();

    void initialization(void);
    void startGame();

private:
    void load_font();
};