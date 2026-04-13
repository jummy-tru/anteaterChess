#include <stdio.h>
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "controller.h"

int main()
{
  Board board;
  Board *boardPtr = &board;
  setupBoard(boardPtr);
  showBoard(boardPtr);

  return 0;
}