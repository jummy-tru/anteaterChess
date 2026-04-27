#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "bots.h"
#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "rules.h"
#include "controller.h"

// Piece-square tables, where white is on the bottom and black is on the top

// Pawns want to promote on the other side of the board
static const int PST_PAWN[ROWS][COLS] = {
    { 60, 60, 60, 60, 60, 60, 60, 60, 60, 60 },
    { 35, 35, 35, 35, 40, 40, 35, 35, 35, 35 },
    { 20, 20, 22, 24, 26, 26, 24, 22, 20, 20 },
    { 10, 10, 14, 18, 20, 20, 18, 14, 10, 10 },
    {  6,  6, 10, 15, 15, 15, 15, 10,  6,  6 },
    {  4,  4,  6,  8, 10, 10,  8,  6,  4,  4 },
    {  1,  2,  3,  4,  5,  5,  4,  3,  2,  1 },
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 }
};

// Knights are better away from the edge of the board
static const int PST_KNIGHT[ROWS][COLS] = {
    {-10,-10,-10,-10,-10,-10,-10,-10,-10,-10},
    {-10,  0,  0,  0,  2,  2,  0, -4 -10,-10},
    {-10,  0,  4, 10, 12, 12, 10,  4, -4,-10},
    {-10,  0, 10, 20, 20, 20, 20, 20,  0,-10},
    {-10,  0, 10, 20, 20, 20, 20, 20,  0,-10},
    {-10, -4,  4, 10, 12, 12, 10,  4, -4,-10},
    {-10,-10, -4,  0,  2,  2,  0, -4,-10,-10},
    {-10,-10,-10,-10,-10,-10,-10,-10,-10,-10}
};

// Long range pieces have better options in the open
static const int PST_BISHOP[ROWS][COLS] = {
    { 0, 0, 0 , 0, 0, 0, 0, 0, 0, 0},
    { 0,  0,  0,  2,  4,  4,  2,  0,  0, 0},
    { 0,  0,  6,  8, 10, 10,  8,  6,  0, 0},
    { 0,  2,  8, 12, 14, 14, 12,  8,  2, 0},
    { 0,  2,  8, 12, 14, 14, 12,  8,  2, 0},
    { 0,  0,  10,  10, 10, 10,  10,  6,  0, 0},
    {0, 0,  0,  2,  4,  4,  2,  0, 0 , 0},
    {0,0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Position doesn't matter much for rooks
static const int PST_ROOK[ROWS][COLS] = {
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0},
    {  0,  0, 0, 0, 0, 0, 0, 0,  0,  0}
};

// Encourage to get off of edge of board
static const int PST_QUEEN[ROWS][COLS] = {
    {-10, -8, -6, -4, -2, -2, -4, -6, -8,-10},
    { -8, -4, -2,  0,  2,  2,  0, -2, -4, -8},
    { -6, -2,  2,  15,  15,  15,  15,  2, -2, -6},
    { -4,  0,  6, 10, 10, 10, 10,  6,  0, -4},
    { -4,  0,  6, 10, 10, 10, 10,  6,  0, -4},
    { -6, -2,  2,  6,  15,  15,  6,  2, -2, -6},
    { -8, -4, -2,  0,  2,  2,  0, -2, -4, -8},
    {-10, -8, -6, -4, -2, -2, -4, -6, -8,-10}
};

static const int PST_KING[ROWS][COLS] = {
    { 20, 22, 20, 18, 16, 16, 18, 20, 22, 24},
    { 20, 16, 12, 10,  8,  0,  0, -5, -5, -5},
    {-16, -10, -6, -4,-2, -2, -4, -6,-10,-16},
    {-12, -10, -2,  0,  -2,  -2,  0, -2, -6,-10},
    {-12, -10, -2,  0,  -2,  -2,  0, -2, -6, 0},
    {-14, -10, -4, -2,  0,  0, -2, -4, -8, 14},
    {-18, 12, -8, -6, -4, -4, -6, -8,-12, 18},
    { 20, 20, 14, 12, 10, 10, 12, 14, 20, 20}
};

// Try to get anteaters off the first row
static const int PST_ANTEATER[ROWS][COLS] = {
    { 0, 0, 0,  0,  0,  0,  0, 0, 0, 0},
    { 10,  10, 12, 10,16, 16, 14, 12,10, 4},
    { 0,  0,  0,  0, 0, 0,  0,  0,  0, 0},
    {  0,  0,  0, 0, 0, 0, 0,  0,  0,  0},
    {  0,  0,  0, 0, 0, 0, 0,  0,  0,  0},
    { 0,  0,  0,  0, 0, 0,  0,  0,  0, 0},
    { 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
    { 0, 0, 0,  0, 0,  0,  0, 0, 0, 0}
};

static int pieceSquareBonus(Piece piece, int row, int col)
{
    int pstRow;

    if (piece.color == WHITE)
    {
        pstRow = row;
    }
    else if (piece.color == BLACK)
    {
        pstRow = (ROWS - 1) - row;
    }
    else
    {
        return 0;
    }

    switch (piece.pieceType)
    {
    case PAWN:
        return PST_PAWN[pstRow][col];
    case KNIGHT:
        return PST_KNIGHT[pstRow][col];
    case BISHOP:
        return PST_BISHOP[pstRow][col];
    case ROOK:
        return PST_ROOK[pstRow][col];
    case QUEEN:
        return PST_QUEEN[pstRow][col];
    case KING:
        return PST_KING[pstRow][col];
    case ANTEATER:
        return PST_ANTEATER[pstRow][col];
    default:
        return 0;
    }
}

// The most simple type of engine, picking legal moves randomly.
Move randomMove(GameController *c)
{
    Color current_turn = get_current_turn(c);
    allLegalMoves(&c->board, current_turn, &c->legal_moves);
    if (c->legal_moves.index <= 0)
    {
        return createMove(-1, -1, -1, -1);
    }
    int random_selection = (rand() % c->legal_moves.index);
    return c->legal_moves.list[random_selection];
}

//Assign material points to piece 
int evalBoard(Board* board){
    int score = 0;
    //Check the entire boards and assgin values
    for (int i = 0; i < ROWS ; i++) {
        for (int j = 0; j < COLS; j++) {
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

            if (p.pieceType != EMPTY)
            {
                int pieceScore = (materialVal * 100) + pieceSquareBonus(p, i, j);
                score += (p.color == WHITE) ? pieceScore : -pieceScore;
            }
        }
    }
    return score;
}


//Depth 1, bot will pick the best value score
Move getBotMove(GameController* c) { 
    MoveList moves;
    moves.index = 0;

    //Check for all possible legal move
    allLegalMoves(&c->board, c->board.currentTurn, &moves);

    //Fall back for checkmate/stalemate
    if(moves.index == 0){
        return createMove(-1,-1,-1,-1);
    }

    //Declaration of variable to see the best move
    Move bestMove = moves.list[0];
    bool isWhite = (c->board.currentTurn == WHITE);
    //If current turn white set the lowest limit, otherwise set the highest
    //The bot will take the highest score 
    int highestScore = (isWhite) ? -99999:99999; //Set the boundary

    /*
    Evaluate every possible moves
    Make a copy of the board and calculate every move with the score
    and will pick the one that is highest(or lowest if black side)
    */

    int alpha = -99999; // Best for white
    int beta = 99999; // Best for black
    for(int i = 0; i < moves.index; i++)
    {
        applyMove(&c->board, moves.list[i]);
        
        int currentScore = minimax(&c->board, 3, !isWhite, alpha, beta);

        undoMove(&c->board);

        //If bot is white, it will take the highest score
        if (isWhite)
        {
            if(currentScore > highestScore)
            {
                highestScore = currentScore;
                bestMove = moves.list[i];
            }

            alpha = (alpha > highestScore) ? alpha : highestScore;

        }
        else{
            //if bot is black, it will take the lowest score
            if(currentScore < highestScore){
                highestScore = currentScore;
                bestMove = moves.list[i];
            }

            beta = (beta < highestScore) ? beta : highestScore;
        }

        if (alpha >= beta)
        {
            break;
        }
    }
    return bestMove;
}

int minimax(Board* board, int depth, bool isWhite, int alpha, int beta) {
    //Base Case to break recursion
    if (depth == 0)
    {
        return evalBoard(board);
    }

    MoveList moves;
    moves.index = 0;

    allLegalMoves(board, isWhite ? WHITE :BLACK, &moves);

    if (moves.index == 0) // Stalemate, or checkmate
    {
        Color side = isWhite ? WHITE : BLACK;
        if (isInCheck(board, side)) // Checkmate
        {
            if (side == BLACK)
            {
                return 50000 + depth;
            }
            else if (side == WHITE)
            {
                return -50000 - depth;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            // Stalemate
            return 0;
        }
    }

    if(isWhite){
        int bestScore = -99999;
        for(int i = 0; i < moves.index;i++){
            applyMove(board, moves.list[i]);

            //Calculate the next turn (black)
            int score = minimax(board, depth - 1, false, alpha, beta);
            undoMove(board);

            bestScore = (score > bestScore) ? score : bestScore;
            alpha = (alpha > bestScore) ? alpha : bestScore;
            if (alpha >= beta)
            {
                break;
            }
        }
        return bestScore;
    }
    else
    {
        int bestScore = 99999;
        for(int i = 0; i < moves.index;i++)
        {
            applyMove(board, moves.list[i]);
        
            //Calulate the next turn (white)
            int score = minimax(board, depth - 1, true, alpha, beta);
            undoMove(board);

            bestScore = (score > bestScore) ? bestScore : score;
            beta = (beta > bestScore) ? bestScore: beta;
            if (alpha >= beta)
            {
                break;
            }
        }
        return bestScore;
    }
}