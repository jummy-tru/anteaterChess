#ifndef BOTS_H
#define BOTS_H
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "controller.h"

int scanBoard(Board* board);
int searchMoves(Board* board, int depth, bool isMax);
Move bestMove(Board* board,int depth);
#endif