#pragma once

#include <vector>
#include "move.h"
#include "raylib.h"

constexpr uint32_t DEFAULT_SCREEN_WIDTH = 800;
constexpr uint32_t DEFAULT_SCREEN_HEIGHT = 450;
constexpr uint32_t DEFAULT_BOARD_DIMENSIONS = 8;

class Game {
private:
    struct Piece board[DEFAULT_BOARD_DIMENSIONS * DEFAULT_BOARD_DIMENSIONS] = {None, PLAYER_WHITE};
    std::vector<Move> history;

    PieceColor turn = PLAYER_WHITE;

public:
    Game() {
        history.reserve(sizeof(Move) * 128);
    }

    void main_loop() {
        while (!WindowShouldClose()) {
            processInput();
            update();
            render();
        }
    }

    void processInput();
    void update();
    void render();

    void setup_board();

    void doMove(Move move);
};