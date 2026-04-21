#ifndef MOVES_H
#define MOVES_H

#include <stdbool.h>
#include "pieces.h"
#include "board.h"

void possibleMoves(Piece* piece, Board* board, int row, int col, MoveList* availableMoves);

void possibleSlidingMoves(Piece* piece, Board* board, int row, int col, MoveList* availableMoves);
void possibleLeapingMoves(Piece* piece, Board* board, int row, int col, MoveList* availableMoves);
void possiblePawnMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMoves);
void possibleKingMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMove);
void possibleAnteaterMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMove);

#endif