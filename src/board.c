// This file is responsible for ACTUALLY moving the pieces as well as
// displaying the contents of the board

#include <stdio.h>
#include <ctype.h>
#include "board.h"

Move createMove(int fromRow, int fromCol, int toRow, int toCol)
{
  Move move;
  move.fromRow = fromRow;
  move.fromCol = fromCol;
  move.toRow = toRow;
  move.toCol = toCol;
  move.isCastling = false;
  move.isEnPassant = false;
  return move;
}

// Changes chess rank like 3rd rank to corresponding array index
int rankToRow(int rank)
{
  return (8 - rank);
}

// Reverse of the above
int rowToRank(int row)
{
  return (8 - row);
}

// Changes chess file like C to corresponding array index USE CAPITAL
int fileToCol(char file)
{
  return (file - 65);
}

// Reverse of the above
char colToFile(int col)
{
  return ((char)(col + 65));
}

void setPieceHasMoved(Board *board, int row, int col, bool hasMoved)
{
  board->squares[row][col].hasMoved = hasMoved;
}

static Piece makePiece(PieceType type, Color color)
{
  Piece p = {0};
  p.pieceType = type;
  p.color = color;
  p.hasMoved = false;
  p.canCastle = false;
  return p;
}

void emptyBoard(Board *board)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      board->squares[i][j] = makePiece(EMPTY, NONE);
    }
  }
}

static char pieceTypeChar(PieceType type)
{
  switch (type)
  {
  case KING:
    return 'K';
  case QUEEN:
    return 'Q';
  case ROOK:
    return 'R';
  case BISHOP:
    return 'B';
  case KNIGHT:
    return 'N';
  case PAWN:
    return 'P';
  case ANTEATER:
    return 'A';
  default:
    return '.';
  }
}

static char pieceColorChar(Color color)
{
  switch (color)
  {
  case WHITE:
    return 'w';
  case BLACK:
    return 'b';
  default:
    return '.';
  }
}

static Piece pieceFromChar(char ch)
{
  Color color;
  PieceType type;

  if (isupper((unsigned char)ch))
  {
    color = WHITE;
  }
  else
  {
    color = BLACK;
  }

  switch (tolower((unsigned char)ch))
  {
  case 'k':
    type = KING;
    break;
  case 'q':
    type = QUEEN;
    break;
  case 'r':
    type = ROOK;
    break;
  case 'b':
    type = BISHOP;
    break;
  case 'n':
    type = KNIGHT;
    break;
  case 'p':
    type = PAWN;
    break;
  case 'a':
    type = ANTEATER;
    break;
  default:
    return makePiece(EMPTY, NONE);
  }

  return makePiece(type, color);
}

int loadBoardFromFEN(Board *board, const char *fen)
{
  int row = 0;
  int col = 0;
  int i = 0;

  for (row = 0; row < ROWS; row++)
  {
    for (col = 0; col < COLS; col++)
    {
      board->squares[row][col] = makePiece(EMPTY, NONE);
    }
  }

  row = 0;
  col = 0;

  while (fen[i] != '\0' && row < ROWS)
  {
    char ch = fen[i];

    if (ch == '/')
    {
      if (col != COLS)
      {
        return 0;
      }
      row++;
      col = 0;
    }
    else if (isdigit((unsigned char)ch))
    {
      int emptyCount = 0;

      while (isdigit((unsigned char)fen[i]))
      {
        emptyCount = emptyCount * 10 + (fen[i] - '0');
        i++;
      }
      i--;

      if (col + emptyCount > COLS)
      {
        return 0;
      }

      col += emptyCount;
    }
    else
    {
      Piece p = pieceFromChar(ch);
      if (p.pieceType == EMPTY)
      {
        return 0;
      }
      if (col >= COLS)
      {
        return 0;
      }

      board->squares[row][col] = p;
      col++;
    }

    i++;
  }

  if (row != ROWS - 1 || col != COLS)
  {
    return 0;
  }

  return 1;
}

// Loads the board using the FEN notation for a board state
void setupBoard(Board *board)
{
  // String Representing the starting state
  const char *startingFEN = "rnbaqkanbr/pppppppppp/10/10/10/10/PPPPPPPPPP/RNBAQKANBR";

  loadBoardFromFEN(board, startingFEN);
  board->currentTurn = WHITE;
  board->moveCount = 0;
  board->isAntEating = false;
}

// Returns the piece object that resides on a particular square
// (can limit permisions eventually
// or have it return a char to simulate private/public)
Piece getPiece(Board *board, int row, int column)
{
  return board->squares[row][column];
}

void movePiece(Board *board, int fromRow, int fromColumn, int toRow, int toColumn)
{
  // Function in charge of moving piece from some place to some place
  // Validation should have already happened in moves so
  // you don't need validation here
  Piece empty;
  empty.color = NONE;
  empty.pieceType = EMPTY;

  Piece movedPiece = board->squares[fromRow][fromColumn];

  board->history[board->moveCount] = createMove(fromRow, fromColumn, toRow, toColumn);

  replacePiece(board, toRow, toColumn, movedPiece);
  replacePiece(board, fromRow, fromColumn, empty);

  board->moveCount++;
}

void applyMove(Board *board, Move move)
{
  Piece movingPiece = getPiece(board, move.fromRow, move.fromCol);
  Piece empty = makePiece(EMPTY, NONE);

  // Store the real move, including special flags
  board->history[board->moveCount] = move;

  // En passant: remove the pawn first
  if (move.isEnPassant)
  {
    replacePiece(board, move.fromRow, move.toCol, empty);
  }

  // Castling: move the rook too
  if (move.isCastling)
  {
    if (move.toCol > move.fromCol)
    {
      // Kingside castle
      Piece rook = getPiece(board, move.fromRow, 9);

      replacePiece(board, move.fromRow, 6, rook);
      board->squares[move.fromRow][6].hasMoved = true;
      replacePiece(board, move.fromRow, 9, empty);
    }
    else
    {
      // Queenside castle
      Piece rook = getPiece(board, move.fromRow, 0);

      replacePiece(board, move.fromRow, 4, rook);
      board->squares[move.fromRow][4].hasMoved = true;
      replacePiece(board, move.fromRow, 0, empty);
    }
  }

  // Move the main piece
  replacePiece(board, move.toRow, move.toCol, movingPiece);
  board->squares[move.toRow][move.toCol].hasMoved = true;

  replacePiece(board, move.fromRow, move.fromCol, empty);

  board->moveCount++;
}

void removePiece(Board *board, int row, int col)
{
  Piece empty = makePiece(EMPTY, NONE);
  replacePiece(board, row, col, empty);
}

// Function replaces a piece during exchanges or promotions or removes
// it in case of En Passant or anteater captures
void replacePiece(Board *board, int row, int column, Piece newPiece)
{
  board->squares[row][column] = newPiece;
}

// This was the text representation of the board before the GUI was implemented
void showBoard(Board *board)
{
  int r, c;

  printf("Move number %d", board->moveCount + 1);
  printf("\n    A    B    C    D    E    F    G    H    I    J\n");
  for (r = 0; r < ROWS; r++)
  {
    printf("  +----+----+----+----+----+----+----+----+----+----+\n");
    printf("%d |", 8 - r);

    for (c = 0; c < COLS; c++)
    {
      Piece p = board->squares[r][c];
      if (p.pieceType == EMPTY)
      {
        printf("    |");
      }
      else
      {
        printf(" %c%c |", pieceColorChar(p.color), pieceTypeChar(p.pieceType));
      }
    }

    printf(" %d\n", 8 - r);
  }
  printf("  +----+----+----+----+----+----+----+----+----+----+\n");
  printf("    A    B    C    D    E    F    G    H    I    J\n\n");
  printf("Current Turn: %s", board->currentTurn == WHITE ? "WHITE" : "BLACK");
}