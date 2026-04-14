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

/*Control Flow Loop
1. Ask user to input moves
2. input validation (check if user input proper notations)
3. isPieceCheck (check if there exist a piece)
4. Piece color check( make sure the piece is the proper color)
5. Check which piece type is it
6. Check if the move is possible for the corresponding Piece type
7. Check if the move is valid via Rule Check (ex: if the move make the king in check)
IF ANY VALIDATION FAIL LOOP BACK TO 1 AND ASK USER AGAIN
8. If all validation is correct then move is legal --> execute the move
9. Check if move remove a piece from the board
    9.2. if true then remove piece first
10. Check if the move is one of the special move(castling, promotion, en passant, etc. )
    10.2 If the move is promotion, change the piece type, then update the move with the move
11. Execute the move
12. Check if checkmate/stalemate
    12.2 If true kill the loop and end the game (END LOOP)
13. Swith Color
LOOP BACK 
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