#pragma once

#include "move.h"
#include "raylib.h"
#include <vector>

constexpr uint32_t DEFAULT_BOARD_DIMENSIONS = 8;

/**
 * Layout data for rendering the board.
 */
struct BoardLayout {
	int space_size;
	int x_start;
	int y_start;
};

class Game {
private:
	struct Piece board[DEFAULT_BOARD_DIMENSIONS * DEFAULT_BOARD_DIMENSIONS] = {{None, PLAYER_WHITE}};
	std::vector<Move> history;

	PieceColor turn = PLAYER_WHITE;

	bool whiteCastleKingSide = true;
	bool whiteCastleQueenSide = true;
	bool blackCastleKingSide = true;
	bool blackCastleQueenSide = true;

	int fifty_move_counter = 0;

	int8_t selected_cell = -1;

public:
	Game() { history.reserve(sizeof(Move) * 128); }

	void render();

	void setup_board();
	void doMove(Move move);

	static BoardLayout computeLayout();
};
