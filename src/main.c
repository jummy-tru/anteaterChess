#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "controller.h"
#include "gui.h"

int main(int argc, char *argv[])
{
  run_gui(argc, argv);

  /*Board board;
  Board *boardPtr = &board;
  setupBoard(boardPtr);
  showBoard(boardPtr);
  /*
  
/*
Loop instruction
1. Ask user to input move
2. Input validation(make sure input is in correct notation)
3. Check if there is a piece in that space
4.Check if the piece is the correct color of the current player turn(if player is white side
then they cannot play a black piece)
5. Check the piece type
6.Check if the move is legal with corresponding piece type
7.Check if the move causes issue such as if it make the king in check of if it is already in check
8.Check if the move is a special move is the special type (castling, en passant, promotion)
9. If the move is to take a piece then remove the taken piece first
IF ANY OF THE VALIDATION FAIL THEN LOOP BACK TO USER PROMPT (1)
10. Excute move if all validation pass
11. Update the board/game state
11. Switch the color
13. Loop
IF THE GAME STATE IS CHECKMATE OR STALEMATE, LOOP IS DEAD
*/

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