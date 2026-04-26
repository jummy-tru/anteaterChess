#ifndef BOTS_H
#define BOTS_H
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"
#include "controller.h"

int evalBoard(Board* board);
Move getBotMove(Board * board);
int minimax();

#endif