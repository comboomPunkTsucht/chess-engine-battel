#pragma once

#include "game.h"
#include <cstdint>

class App {
    public:
        Game *mCurrentGame;

        void main_loop();
        void processInput();
        void update();
        void render();

        void initialization(void);
        void startGame();

    private:
        void load_font();
};
