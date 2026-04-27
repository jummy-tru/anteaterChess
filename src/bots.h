#ifndef BOTS_H
#define BOTS_H
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"
#include "controller.h"

Move randomMove(GameController *c);
int evalBoard(Board* board);
Move getBotMove(Board * board);
int minimax(Board* board, int depth, bool isWhite);

#endif