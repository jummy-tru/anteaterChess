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
    break;
  case (QUEEN):
    possibleSlidingMoves(piece, board, row, col, true, true, availableMoves);
    break;
  case (BISHOP):
    possibleSlidingMoves(piece, board, row, col, false, true, availableMoves);
  }
}

void addNeighbors(Piece* piece, Board* board, int row, int col, int rowOffset, int colOffset, MoveList *availableMoves)
{
  // Take direction from user and increment in that direction
  int currentRow = row + rowOffset;
  int currentCol = col + colOffset;
  while (currentRow >= 0 && currentRow <= 7 && currentCol >= 0 && currentCol <= 9)
  {
    // If the squarein direction is occupied by opposite color
    if (getPiece(board, currentRow, currentCol).pieceType != EMPTY && 
      getPiece(board, currentRow, currentCol).color != piece->color)
    {
      availableMoves->list[availableMoves->index] = 
        createMove(row, col, currentRow, currentCol);
      availableMoves->index++;
      break;
    }
    // If the square in direction is occupied by same color
    else if (getPiece(board, currentRow, currentCol).pieceType != EMPTY && 
      getPiece(board, currentRow, currentCol).color == piece->color)
    {
      break;
    }
    // Square to the left is not occupied
    else {
      availableMoves->list[availableMoves->index] = 
        createMove(row, col, currentRow, currentCol);
      availableMoves->index++;
      currentRow += rowOffset;
      currentCol += colOffset;
    }
  }
}

// diagonal or linear movement
// rooks, bishops, and queen can use this one  
void possibleSlidingMoves(Piece* piece, Board* board, int row, int col, bool isLinear, bool isDiagonal, MoveList *availableMoves)
{
	if (isLinear)
	{
		// Check left side
		addNeighbors(piece, board, row, col, 0, -1, availableMoves);

    // Check right side
		addNeighbors(piece, board, row, col, 0, 1, availableMoves);

    // Check above
    addNeighbors(piece, board, row, col, -1, 0, availableMoves);

    // Check below
    addNeighbors(piece, board, row, col, 1, 0, availableMoves);
	}

  if (isDiagonal)
  {  // Check top left
    addNeighbors(piece, board, row, col, -1, -1, availableMoves);

    // Check top right
    addNeighbors(piece, board, row, col, -1, 1, availableMoves);

    // Check bottom left
    addNeighbors(piece, board, row, col, 1, -1, availableMoves);

    // Check bottom right
    addNeighbors(piece, board, row, col, 1, 1, availableMoves);
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