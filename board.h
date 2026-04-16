#ifndef BOARD_H
#define BOARD_H

#include "pieces.h"

#define ROWS 8
#define COLS 10

typedef struct {
	int fromRow;
	int fromCol;
	int toRow;
	int toCol;
	bool isCastling;
	bool isEnPassant;
} Move;

typedef struct {
	Move list[500];
	int index;
} MoveList;

typedef struct {
  Piece squares[8][10];
  Color currentTurn;
  Move history[512];
  int moveCount;
} Board;

int rankToRow(int rank);
int rowToRank(int row);
int fileToCol(char file);
char colToFile(int col);

void emptyBoard(Board* board);
void setupBoard(Board* board);
int loadBoardFromFEN(Board *board, const char *fen);
Piece getPiece(Board* board, int row, int column);
void movePiece(Board* board, int fromRow, int fromColumn, int toRow, int toColumn);
void removePiece(Board *board, int row, int col);
void replacePiece(Board* board, int row, int column, Piece newPiece);
void showBoard(Board* board);

#endif