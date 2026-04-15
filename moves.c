// File in charge of computing moves and validating them

#include "moves.h"
#include "rules.h"
#include <stdlib.h>

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
  case PAWN:
    possiblePawnMoves(piece, board, row, col, availableMoves);
    break;
  default:
    break;
  }
}

void addNeighbors(Piece *piece, Board *board, int row, int col, int rowOffset, int colOffset, MoveList *availableMoves)
{
  // Take direction from user and increment in that direction
  int currentRow = row + rowOffset;
  int currentCol = col + colOffset;
  while (isInsideBoard(currentRow, currentCol))
  {
    // If the squarein direction is occupied by opposite color

    if (isEnemyPiece(getPiece(board, currentRow, currentCol), piece->color))
    {
      availableMoves->list[availableMoves->index] =
          createMove(row, col, currentRow, currentCol);
      availableMoves->index++;
      break;
    }
    // If the square in direction is occupied by same color
    else if (isOwnPiece(getPiece(board, currentRow, currentCol), piece->color))
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
    isLinear = true;
    isDiagonal = true;
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
      {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};

  // Check each possible jump
  for (int i = 0; i < 8; i++)
  {
    // Find target squares
    int newRow = row + offsets[i][0];
    int newCol = col + offsets[i][1];

    // Skip this move if it goes off the board
    if (!isInsideBoard(newRow, newCol))
      continue;

    // Get the piece on the target square and check if it is empty or not a friendly piece and add it.
    Piece target = getPiece(board, newRow, newCol);

    if (!isOwnPiece(target, piece->color))
    {
      availableMoves->list[availableMoves->index] = createMove(row, col, newRow, newCol);
      availableMoves->index++;
    }
  }
}

// Ant (Pawn) movement
void possiblePawnMoves(Piece *piece, Board *board, int row, int col, MoveList *availableMoves)
{
  int direction;
  int startRow;
  int oneStepRow;
  int twoStepRow;
  int leftCaptureCol = col - 1;
  int rightCaptureCol = col + 1;

  // White moves upward on the board array (toward smaller row index)
  // Black moves downward on the board array (toward larger row index)
  if (piece->color == WHITE)
  {
    direction = -1;
    startRow = 6; // white pawns start on rank 2
  }
  else if (piece->color == BLACK)
  {
    direction = 1;
    startRow = 1; // black pawns start on rank 7
  }
  else
  {
    return;
  }

  oneStepRow = row + direction;
  twoStepRow = row + 2 * direction;

  // Forward one square: only if inside board and empty
  if (isInsideBoard(oneStepRow, col) && getPiece(board, oneStepRow, col).pieceType == EMPTY)
  {
    availableMoves->list[availableMoves->index] = createMove(row, col, oneStepRow, col);
    availableMoves->index++;

    // Forward two squares: only from starting row,
    // and only if both intermediate and destination are empty
    if (row == startRow && isInsideBoard(twoStepRow, col) && getPiece(board, twoStepRow, col).pieceType == EMPTY)
    {
      availableMoves->list[availableMoves->index] = createMove(row, col, twoStepRow, col);
      availableMoves->index++;
    }
  }

  // Diagonal capture to the left
  if (isInsideBoard(oneStepRow, leftCaptureCol) && isEnemyPiece(getPiece(board, oneStepRow, leftCaptureCol), piece->color))
  {
    availableMoves->list[availableMoves->index] = createMove(row, col, oneStepRow, leftCaptureCol);
    availableMoves->index++;
  }

  // Diagonal capture to the right
  if (isInsideBoard(oneStepRow, rightCaptureCol) && isEnemyPiece(getPiece(board, oneStepRow, rightCaptureCol), piece->color))
  {
    availableMoves->list[availableMoves->index] = createMove(row, col, oneStepRow, rightCaptureCol);
    availableMoves->index++;
  }

  // En passant
  // Only possible if at least one move has already been played
  if (board->moveCount > 0)
  {
    Move lastMove = board->history[board->moveCount - 1];

    Piece lastMovedPiece = getPiece(board, lastMove.toRow, lastMove.toCol);

    // Last move must be an enemy pawn that moved two squares
    if (lastMovedPiece.pieceType == PAWN && lastMovedPiece.color != piece->color && abs(lastMove.toRow - lastMove.fromRow) == 2)
    {
      // That pawn must now be directly next to this pawn
      if (lastMove.toRow == row && abs(lastMove.toCol - col) == 1)
      {
        int enPassantRow = row + direction;
        int enPassantCol = lastMove.toCol;

        // Destination must be on board and empty
        if (isInsideBoard(enPassantRow, enPassantCol) && getPiece(board, enPassantRow, enPassantCol).pieceType == EMPTY)
        {
          availableMoves->list[availableMoves->index] = createMove(row, col, enPassantRow, enPassantCol);
          availableMoves->index++;
        }
      }
    }
  }

}

void possibleAnteaterMoves()
{
  // good luck
}