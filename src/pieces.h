#ifndef PIECES_H
#define PIECES_H
#include <stdbool.h>

// File responsible for defining what piece struct is and what values
// are allowed by using enumerations
typedef enum {
  EMPTY,
  PAWN,
  KING,
  QUEEN,
  KNIGHT,
  BISHOP,
  ANTEATER,
  ROOK
} PieceType;

typedef enum {
  BLACK,
  WHITE,
  NONE
} Color;

typedef struct {
  PieceType pieceType;
  Color color;
  bool hasMoved;
  bool canCastle;
} Piece;

#endif