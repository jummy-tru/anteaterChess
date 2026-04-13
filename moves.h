#ifndef MOVES_H
#define MOVES_H

#include "pieces.h"

void makeMoves(Piece* piece);

void makeSlidingMoves();
void makeLeapingMoves();
void makeAnteaterMoves();

#endif