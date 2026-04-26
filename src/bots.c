#include <stdio.h>
#include <stdbool.h>
#include "bots.h"
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"
#include "controller.h"

//Assign material points to piece 
int evalBoard(Board* board){
    int score = 0;
    //Check the entire boards and assgin values
    for(int i = 0; i < ROWS ; i++){
        for (int j = 0; j < COLS; j++){
            Piece p = getPiece(board, i,j);

            int materialVal = 0;

            switch (p.pieceType)
            {
            case PAWN:
                materialVal = 1;
                break;
            case ANTEATER:
                materialVal = 2; 
                break;
            case BISHOP:
                materialVal = 4;
                break;
            case KNIGHT:
                materialVal = 4;
                break;
            case ROOK: 
                materialVal = 6;
                break;
            case QUEEN: 
                materialVal = 15;
                break;
            case KING:
                materialVal = 900;
                break;
            default:
                break;
            }
            score += (p.color == WHITE) ? materialVal:-materialVal; //If white piece postive, black is negative value
        }
    }
}


//Depth 1, bot will pick the best value score
Move getBotMove(Board* board){
    MoveList moves;
    moves.index = 0;

    //Check for all possible legal move
    allLegalMoves(board, board->currentTurn, &moves);

    //Fall back for checkmate/stalemate
    if(moves.index == 0){
        return createMove(-1,-1,-1,-1);

    }

    //Declaration of variable to see the best move
    Move bestMove = moves.list[0];
    bool isWhite = (board->currentTurn== WHITE);
    //If current turn white set the lowest limit, otherwise set the highest
    //The bot will take the highest score 
    int highestScore = (isWhite) ? -99999:99999; //Set the boundary

    /*
    Evaluate every possible moves
    Make a copy of the board and calculate every move with the score
    and will pick the one that is highest(or lowest if black side)
    */
    for(int i = 0; i<moves.index;i++){
        Board temp;
        copyBoard(&temp,board);
        applyMove(&temp,moves.list[i]);

        int currentScore = minimax(&temp,2, !isWhite);

        //If bot is white, it will take the highest score
        if(isWhite){
            if(currentScore > highestScore){
                highestScore = currentScore;
                bestMove = moves.list[i];
            }

        }
        else{
            //if bot is black, it will take the lowest score
            if(currentScore < highestScore){
                highestScore = currentScore;
                bestMove = moves.list[i];
            }
        }
       
    }

    return bestMove;

    
}

int minimax(Board* board, int depth, bool isWhite){

    //Base Case to break recursion
    if(depth == 0||isCheckmate(board,WHITE) || isCheckmate(board,BLACK)){
        return evalBoard(board);
    }

    MoveList moves;
    Color turn;
    moves.index = 0;

    if (isWhite){
        turn == WHITE;
    }
    else{
        turn == BLACK;
    }

    allLegalMoves(board, isWhite ? WHITE :BLACK, &moves);

    if(moves.index == 0){
        return 0; //No legal moves, so stalemate
    }

    if(isWhite){
        int bestScore = -99999;
        for(int i = 0; i < moves.index;i++){
            Board temp;
            copyBoard(&temp, board);
            applyMove(&temp,moves.list[i]);
        

            //Calculate the next turn (black)
            int score = minimax(&temp,depth-1,false);
            if(score >bestScore){
                bestScore = score;
            }
        }
        return bestScore;
    }
    else{
        int bestScore = 99999;
        for(int i = 0; i < moves.index;i++){
            Board temp;
            copyBoard(&temp, board);
            applyMove(&temp,moves.list[i]);
        
            //Calulate the next turn (white)
            int score = minimax(&temp,depth-1,true);
            if(score <bestScore){
                bestScore = score;
            }
        }
        return bestScore;
    }

}