// This file is responsible for ACTUALLY moving the pieces as well as
// displaying the contents of the board

#include <stdio.h>
#include <ctype.h>
#include "board.h"

void emptyBoard(Board *board)
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      board->squares[i][j].color = NONE;
      board->squares[i][j].pieceType = EMPTY;
    }
  }
}

static Piece makePiece(PieceType type, Color color)
{
  Piece p;
  p.pieceType = type;
  p.color = color;
  return p;
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
}

// Function replaces a piece during exchanges or promotions or removes
// it in case of En Passant or anteater captures
void replacePiece(Board *board, int row, int column, PieceType newPiece)
{
}

void showBoard(Board *board)
{
  int r, c;

  printf("\n    A   B   C   D   E   F   G   H   I   J\n");
  for (r = 0; r < ROWS; r++)
  {
    printf("  +---+---+---+---+---+---+---+---+---+---+\n");
    printf("%d |", 8 - r);

    for (c = 0; c < COLS; c++)
    {
      Piece p = board->squares[r][c];
      if (p.pieceType == EMPTY)
      {
        printf("   |");
      }
      else
      {
        printf(" %c%c|", pieceColorChar(p.color), pieceTypeChar(p.pieceType));
      }
    }

    printf(" %d\n", 8 - r);
  }
  printf("  +---+---+---+---+---+---+---+---+---+---+\n");
  printf("    A   B   C   D   E   F   G   H   I   J\n\n");
}