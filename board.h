#ifndef BOARD_H
#define BOARD_H

#include "pieces.h"

#define ROWS 8
#define COLS 10

typedef struct {
  Piece squares[8][10];
} Board;

int rankToRow(int rank);
int rowToRank(int row);
int fileToCol(char file);
char colToFile(int col);

void emptyBoard(Board* board);
void setupBoard(Board* board);
int loadBoardFromFEN(Board *board, const char *fen);
Piece getPiece(Board* board, int row, int column);
void movePiece(Board* board, int fromRow, int fromColumn, 
                int toRow, int toColumn);
void replacePiece(Board* board, int row, int column, PieceType newPiece);
void showBoard(Board* board);

#endif