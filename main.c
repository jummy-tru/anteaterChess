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

  /*
  // TEST CASE #1 
  // can move test cases somewhere else eventually
  // This test creates a queen in the middle of the board and checks for valid moves
  Board board;
  Board *boardPtr = &board;
  loadBoardFromFEN(boardPtr, "1rnbqkbnr1/pppppppppp/10/5Q4/10/10/PPPPPPPPPP/1RNBQKBNR1");
  MoveList ml;
  ml.index = 0;
  MoveList *mlPtr = &ml;
  showBoard(boardPtr);

  int queenRow = rankToRow(5);
  int queenCol = fileToCol('F');
  Piece queen = getPiece(boardPtr,queenRow,queenCol);
  Piece *queenPtr = &queen;
  possibleMoves(queenPtr, boardPtr, queenRow, queenCol, mlPtr);

  for (int i = 0; i < mlPtr->index; i++)
  {
    printf("From: %c%i To: %c%i \n", colToFile(mlPtr->list[i].fromCol), rowToRank(mlPtr->list[i].fromRow), colToFile(mlPtr->list[i].toCol), rowToRank(mlPtr->list[i].toRow));
  }
  return 0;
  */
}