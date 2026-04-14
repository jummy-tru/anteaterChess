// File in charge of computing moves and validating them

#include "moves.h"

Move createMove(int fromRow, int fromCol, int toRow, int toCol)
{
	Move move;
	move.fromRow = fromRow;
	move.fromCol = fromCol;
	move.toRow = toRow;
	move.toCol = toCol;
	return move;
}

void possibleMoves(Piece* piece, Board* board, int row, int col, MoveList *availableMoves)
{
	switch (piece->pieceType)
	{
	case (ROOK):
		possibleSlidingMoves(piece, board, row, col, true, false, availableMoves);
	}
}

// "simple" diagonal or linear movement
// rooks, bishops, king, and queen can use this one  
// can refactor into linear and diagonals separately if you want
void possibleSlidingMoves(Piece* piece, Board* board, int row, int col, bool isLinear, bool isDiagonal, MoveList *availableMoves)
{
	if (isLinear)
	{
		// Check left side
		int currentCol = row - 1;
		while (currentCol >= 0 && currentCol <= 9)
		{
			availableMoves->list[availableMoves->index] = createMove(row, col, row, currentCol);
			availableMoves->index++;
			if (getPiece(board, row, currentCol).pieceType != EMPTY)
			{
				break;
			}
			else
			{
				currentCol--;
			}
		}
	}
}

// Knight movement
void possibleLeapingMoves()
{

}

void possibleAnteaterMoves()
{
  // good luck
}