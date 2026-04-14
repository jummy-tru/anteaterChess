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

void possibleMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMoves)
{
  switch (piece->pieceType)
  {
  case (ROOK):
  case (QUEEN):
  case (BISHOP):
    possibleSlidingMoves(piece, board, row, col, availableMoves);
    break;
  case (KNIGHT):
    possibleLeapingMoves(piece, board, row, col, availableMoves);
    break;
  }
}

void addNeighbors(Piece *piece, Board *board, int row, int col, int rowOffset, int colOffset, MoveList *availableMoves)
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
    else
    {
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
void possibleSlidingMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMoves)
{
  bool isLinear = false;
  bool isDiagonal = false;
  switch (piece->pieceType)
  {
  case QUEEN:
    isLinear = true; isDiagonal = true;
    break;
  case ROOK:
    isLinear = true;
    break;
  case BISHOP:
    isDiagonal = true;
    break;
  }

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
  { // Check top left
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
void possibleLeapingMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMoves)
{
  // All 8 possible knight jumps;
  // 2 in one direction and 1 perpendicular
	int offsets[8][2] = {
	  {-2, -1}, {-2,  1},
		{-1, -2}, {-1,  2},
		{ 1, -2}, { 1,  2},
		{ 2, -1}, { 2,  1}
	};

  // Check each possible jump
  for (int i = 0; i < 8; i++)
  {
    // Find target squares
    int newRow = row + offsets[i][0];
    int newCol = col + offsets[i][1];

    // Skip this move if it goes off the board
    if (newRow < 0 || newRow > 7 || newCol < 0 || newCol > 9)
    {
      continue;
    }

    // Get the piece ont the target square and check if it is empty or not a friendly piece and add it.
    Piece target = getPiece(board, newRow, newCol);

    if (target.pieceType == EMPTY || target.color != piece->color)
    {
      availableMoves->list[availableMoves->index] = createMove(row, col, newRow, newCol);
      availableMoves->index++;
    }
  }
}

void possibleAnteaterMoves()
{
  // good luck
}