#ifndef MOVES_H
#define MOVES_H

#include "pieces.h"
#include "board.h"
#include <stdbool.h>

typedef struct {
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;
} Move;

typedef struct {
	Move list[500];
	int index;
} MoveList;

Move createMove(int fromRow, int fromCol, int toRow, int toCol);
void possibleMoves(Piece* piece, Board* board, int row, int col, MoveList* availableMoves);

void possibleSlidingMoves(Piece* piece, Board* board, int row, int col, bool isLinear, bool isDiagonal, MoveList* availableMoves);
void possibleLeapingMoves();
void possibleAnteaterMoves();

#endif