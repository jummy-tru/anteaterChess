#ifndef RULES_H
#define RULES_H

#include <stdbool.h>
#include "board.h"
#include "moves.h"
#include "pieces.h"

// Basic legality checks
bool isInsideBoard(int row, int col);
bool isOwnPiece(Piece piece, Color color);
bool isEnemyPiece(Piece piece, Color color);

// Move validation
bool isPseudoLegalMove(Board *board, Move move);
bool isLegalMove(Board *board, Move move);

// King safety / attack detection
bool pieceAttacksSquare(Board *board, int fromRow, int fromCol, int targetRow, int targetCol);
bool isPathClear(Board *board, int fromRow, int fromCol, int targetRow, int targetCol);
bool isSquareAttacked(Board *board, int targetRow, int targetCol, Color byColor);
bool findKing(Board *board, Color kingColor, int *kingRow, int *kingCol);
bool isInCheck(Board *board, Color color);
bool isInCheckAfterMove(Board *board, Move move);

// Full legal move generation
void legalMovesForPiece(Board *board, int row, int col, MoveList *legalMoves);
void allLegalMoves(Board *board, Color color, MoveList *legalMoves);

// Endgame checks
bool hasAnyLegalMoves(Board *board, Color color);
bool isCheckmate(Board *board, Color color);
bool isStalemate(Board *board, Color color);

// Board simulation helper
void copyBoard(Board *dest, Board *src);

#endif