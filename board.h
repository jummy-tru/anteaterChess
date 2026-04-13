#ifndef BOARD_H
#define BOARD_H

#include "pieces.h"

typedef struct {
  Piece squares[8][10];
} Board;

void emptyBoard(Board* board);
void setupBoard(Board* board);
Piece getPiece(Board* board, int row, int column);
void movePiece(Board* board, int fromRow, int fromColumn, 
                int toRow, int toColumn);
void replacePiece(Board* board, int row, int column, PieceType newPiece);
void showBoard(Board* board);

#endif