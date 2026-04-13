// This file is responsible for ACTUALLY moving the pieces as well as
// displaying the contents of the board

#include <stdio.h>
#include "board.h"

void emptyBoard(Board* board)
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

void setupBoard(Board* board)
{
  // Empty current board
  emptyBoard(board);

  // Initialize white pawns
  for (int i = 0; i < 10; i++)
  {
    // A through J 2
    board->squares[1][i].color = WHITE;
    board->squares[1][i].pieceType = PAWN;
  }

  // Initialize white king
  // F1 (row 1, column 6)
  board->squares[0][5].color = WHITE;
  board->squares[0][5].pieceType = KING;

  // Initialize white queen
  // E1 (row 1, column 5)
  board->squares[0][4].color = WHITE;
  board->squares[0][4].pieceType = QUEEN;

  // Initialize white anteaters
  // D1 and G1 (row 1, columns 4 and 7)
  board->squares[0][3].color = WHITE;
  board->squares[0][3].pieceType = ANTEATER;
  board->squares[0][6].color = WHITE;
  board->squares[0][6].pieceType = ANTEATER;

  // Initialize white bishops
  // C1 and H1 (row 1, columns 3 and 8)
  board->squares[0][2].color = WHITE;
  board->squares[0][2].pieceType = BISHOP;
  board->squares[0][7].color = WHITE;
  board->squares[0][7].pieceType = BISHOP;

  // Initialize white knights
  // B1 and I1 (row 1, columns 2 and 9)
  board->squares[0][1].color = WHITE;
  board->squares[0][1].pieceType = KNIGHT;
  board->squares[0][8].color = WHITE;
  board->squares[0][8].pieceType = KNIGHT;

  //Initialize white rooks
  // A1 and J1 (row 1, columns 1 and 10)
  board->squares[0][0].color = WHITE;
  board->squares[0][0].pieceType = ROOK;
  board->squares[0][9].color = WHITE;
  board->squares[0][9].pieceType = ROOK;

  // Initialize black pawns
  for (int i = 0; i < 10; i++)
  {
    // A through J 7
    board->squares[6][i].color = BLACK;
    board->squares[6][i].pieceType = PAWN;
  }

  // Initialize black king
  // F8 (row 8, column 6)
  board->squares[7][5].color = BLACK;
  board->squares[7][5].pieceType = KING;

  // Initialize black queen
  // E8 (row 8, column 5)
  board->squares[7][4].color = BLACK;
  board->squares[7][4].pieceType = QUEEN;

  // Initialize black anteaters
  // D8 and G8 (row 8, columns 4 and 7)
  board->squares[7][3].color = BLACK;
  board->squares[7][3].pieceType = ANTEATER;
  board->squares[7][6].color = BLACK;
  board->squares[7][6].pieceType = ANTEATER;

  // Initialize black bishops
  // C8 and H8 (row 8, columns 3 and 8)
  board->squares[7][2].color = BLACK;
  board->squares[7][2].pieceType = BISHOP;
  board->squares[7][7].color = BLACK;
  board->squares[7][7].pieceType = BISHOP;

  // Initialize black knights
  // B8 and I8 (row 8, columns 2 and 9)
  board->squares[7][1].color = BLACK;
  board->squares[7][1].pieceType = KNIGHT;
  board->squares[7][8].color = BLACK;
  board->squares[7][8].pieceType = KNIGHT;

  // Initialize black rooks
  // A8 and J8 (row 8, columns 1 and 10)
  board->squares[7][0].color = BLACK;
  board->squares[7][0].pieceType = ROOK;
  board->squares[7][9].color = BLACK;
  board->squares[7][9].pieceType = ROOK;
}

// Returns the piece object that resides on a particular square 
// (can limit permisions eventually 
// or have it return a char to simulate private/public)
Piece getPiece(Board* board, int row, int column)
{
  return board->squares[row][column];
}

void movePiece(Board* board, int fromRow, int fromColumn, 
                int toRow, int toColumn)
{
  // Function in charge of moving piece from some place to some place
  // Validation should have already happened in moves so 
  //you don't need validation here
}

// Function replaces a piece during exchanges or promotions or removes 
// it in case of En Passant or anteater captures
void replacePiece(Board* board, int row, int column, PieceType newPiece)
{
  
}

void showBoard(Board* board)
{
  char pieceColorText[8][10]; // char array storing w, b, or ' ' (none)
  char pieceTypeText[8][10]; // char array storing R,N,B,A,Q,K,P or ' ' (none)
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 10; j++)
    {
      Piece currentPiece = getPiece(board, i, j);
      if (currentPiece.color == BLACK)
      {
        pieceColorText[i][j] = 'b';
      }
      else if (currentPiece.color == WHITE)
      {
        pieceColorText[i][j] = 'w';
      }
      else
      {
        pieceColorText[i][j] = ' ';
      }

      switch (currentPiece.pieceType)
      {
      case PAWN:
        pieceTypeText[i][j] = 'P';
        break;
      case ROOK:
        pieceTypeText[i][j] = 'R';
        break;
      case KNIGHT:
        pieceTypeText[i][j] = 'N';
        break;
      case BISHOP:
        pieceTypeText[i][j] = 'B';
        break;
      case ANTEATER:
        pieceTypeText[i][j] = 'A';
        break;
      case QUEEN:
        pieceTypeText[i][j] = 'Q';
        break;
      case KING:
        pieceTypeText[i][j] = 'K';
        break;
      default:
        pieceTypeText[i][j] = ' ';
      }
    }
  }

  for (int i = 8; i > 0; i--)
  {
    printf("  +----+----+----+----+----+----+----+----+----+----+\n");
    // if yall can think of a better way to refactor this be my guest.
    // it works tho! dont break it!
    printf("%i | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c | %c%c |\n", i, pieceColorText[i-1][0], pieceTypeText[i-1][0], pieceColorText[i-1][1], pieceTypeText[i-1][1], pieceColorText[i-1][2], pieceTypeText[i-1][2], pieceColorText[i-1][3], pieceTypeText[i-1][3], pieceColorText[i-1][4], pieceTypeText[i-1][4], pieceColorText[i-1][5], pieceTypeText[i-1][5], pieceColorText[i-1][6], pieceTypeText[i-1][6], pieceColorText[i-1][7], pieceTypeText[i-1][7], pieceColorText[i-1][8], pieceTypeText[i-1][8], pieceColorText[i-1][9], pieceTypeText[i-1][9]);
  }
  printf("  +----+----+----+----+----+----+----+----+----+----+\n");
  printf("    A    B    C    D    E    F    G    H    I    J\n");
}