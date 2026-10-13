#include "game.h"
#include <raylib.h>

class Game {
    struct Piece board[DEFAULT_BOARD_DIMENSIONS * DEFAULT_BOARD_DIMENSIONS] = {{(Piece){None, PLAYER_WHITE, 0}}};
    

    public: Game() {

    }

    void main_loop() {
        while (!WindowShouldClose()) {
            processInput(this);
            update(this);
            render(this);
        }
    }

    void processInput(Game* game);
    void update(Game* game);
    void render(Game* game);

    void setup_board() {
        board[0].type = Rook;   board[0].color = PLAYER_WHITE;
        board[1].type = Knight; board[1].color = PLAYER_WHITE;
        board[2].type = Bishop; board[2].color = PLAYER_WHITE;
        board[3].type = Queen;  board[3].color = PLAYER_WHITE;
        board[4].type = King;   board[4].color = PLAYER_WHITE;
        board[5].type = Bishop; board[5].color = PLAYER_WHITE;
        board[6].type = Knight; board[6].color = PLAYER_WHITE;
        board[7].type = Rook;   board[7].color = PLAYER_WHITE;

        for (int i = 8; i < 16; i++) {
            board[i].type = Pawn;
            board[i].color = PLAYER_WHITE;
        }

        for (int i = 16; i < 48; i++) {
            board[i].type = None;
            board[i].color = PLAYER_WHITE;
        }

        for (int i = 48; i < 56; i++) {
            board[i].type = Pawn;
            board[i].color = PLAYER_BLACK;
        }

        board[56].type = Rook;   board[56].color = PLAYER_BLACK;
        board[57].type = Knight; board[57].color = PLAYER_BLACK;
        board[58].type = Bishop; board[58].color = PLAYER_BLACK;
        board[59].type = Queen;  board[59].color = PLAYER_BLACK;
        board[60].type = King;   board[60].color = PLAYER_BLACK;
        board[61].type = Bishop; board[61].color = PLAYER_BLACK;
        board[62].type = Knight; board[62].color = PLAYER_BLACK;
        board[63].type = Rook;   board[63].color = PLAYER_BLACK;
    }
};