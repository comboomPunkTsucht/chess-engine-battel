#pragma once

#include "move.h"
#include "raylib.h"
#include <vector>

constexpr uint32_t DEFAULT_BOARD_DIMENSIONS = 8;

class Game {
  private:
    struct Piece board[DEFAULT_BOARD_DIMENSIONS * DEFAULT_BOARD_DIMENSIONS] = {
        {None, PLAYER_WHITE}};
    std::vector<Move> history;

    PieceColor        turn = PLAYER_WHITE;

  public:
    Game() { history.reserve(sizeof(Move) * 128); }

    void render();

    void setup_board();
    void doMove(Move move);
};