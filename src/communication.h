#pragma once

#include "move.h"
#include "piece.h"
#include "game.h"

Move getMove();
void sendMove(Move move);

Game getGame();
void sendGame(Game game);