#include "rules.h"

// Returns true if the given row and column are inside the 8x10 board
bool isInsideBoard(int row, int col)
{
    return (row >= 0 && row < ROWS && col >= 0 && col < COLS);
}

// Returns true if the piece is not empty and belongs to the given color
bool isOwnPiece(Piece piece, Color color)
{
    return (piece.pieceType != EMPTY && piece.color == color);
}

// Returns true if the piece is not empty, is not NONE, and belongs to the opponent
bool isEnemyPiece(Piece piece, Color color)
{
    return (piece.pieceType != EMPTY && piece.color != color && piece.color != NONE);
}

// Copies the full board state from src into dest
// Since Board is a struct, simple assignment copies all fields
void copyBoard(Board *dest, Board *src)
{
    *dest = *src;
}

// Checks whether a move is pseudo-legal
// Pseudo-legal means:
// 1. the source and destination are on the board
// 2. there is actually a piece at the source square
// 3. the destination appears in that piece's generated move list
// This does NOT check whether the move leaves the king in check
bool isPseudoLegalMove(Board *board, Move move)
{
    Piece piece;
    MoveList possible;

    // Start with an empty move list
    possible.index = 0;

    // Reject moves with source or destination off the board
    if (!isInsideBoard(move.fromRow, move.fromCol) || !isInsideBoard(move.toRow, move.toCol))
    {
        return false;
    }

    // Get the piece being moved
    piece = getPiece(board, move.fromRow, move.fromCol);

    // Reject the move if there is no piece on the source square
    if (piece.pieceType == EMPTY)
    {
        return false;
    }

    // Generate all reachable moves for this piece
    possibleMoves(&piece, board, move.fromRow, move.fromCol, &possible);

    // Check whether the desired destination appears in the generated list
    for (int i = 0; i < possible.index; i++)
    {
        if (possible.list[i].toRow == move.toRow &&
            possible.list[i].toCol == move.toCol)
        {
            return true;
        }
    }

    // Destination was not found in the pseudo-legal list
    return false;
}

// Searches the board for the king of the specified color
// If found, stores its location in kingRow and kingCol and returns true
// If not found, returns false
bool findKing(Board *board, Color kingColor, int *kingRow, int *kingCol)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Piece p = getPiece(board, r, c);

            if (p.pieceType == KING && p.color == kingColor)
            {
                *kingRow = r;
                *kingCol = c;
                return true;
            }
        }
    }

    return false;
}

// Checks whether a target square is attacked by any piece of byColor
// It does this by generating possible moves for every piece of that color
// and checking if any of those moves land on the target square
bool isSquareAttacked(Board *board, int targetRow, int targetCol, Color byColor)
{
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Piece p = getPiece(board, r, c);

            // Skip empty squares and pieces of the wrong color
            if (p.pieceType == EMPTY || p.color != byColor)
            {
                continue;
            }

            // Generate attack squares for this piece
            MoveList attacks;
            attacks.index = 0;

            possibleMoves(&p, board, r, c, &attacks);

            // Check whether any generated move attacks the target square
            for (int i = 0; i < attacks.index; i++)
            {
                if (attacks.list[i].toRow == targetRow &&
                    attacks.list[i].toCol == targetCol)
                {
                    return true;
                }
            }
        }
    }

    // No attacking piece was found
    return false;
}

// Returns true if the specified color's king is currently in check
// A king is in check if the enemy attacks the king's square
bool isInCheck(Board *board, Color color)
{
    int kingRow, kingCol;
    Color enemyColor = (color == WHITE) ? BLACK : WHITE;

    // If the king cannot be found, return false for now
    if (!findKing(board, color, &kingRow, &kingCol))
    {
        return false;
    }

    // Check whether the enemy attacks the king's position
    return isSquareAttacked(board, kingRow, kingCol, enemyColor);
}

// Simulates a move on a copied board and checks whether
// the moving side would be in check after making that move
bool isInCheckAfterMove(Board *board, Move move)
{
    Board temp;
    Piece movingPiece;

    // Make a copy so the real board is not modified
    copyBoard(&temp, board);

    // Save the color of the piece being moved
    movingPiece = getPiece(&temp, move.fromRow, move.fromCol);

    // Apply the move on the copied board
    movePiece(&temp, move.fromRow, move.fromCol, move.toRow, move.toCol);

    // Check if that side's king is in check after the move
    return isInCheck(&temp, movingPiece.color);
}

// Checks whether a move is fully legal
// A legal move must:
// 1. move a real piece
// 2. belong to the side whose turn it is
// 3. be pseudo-legal by movement rules
// 4. not leave that side's king in check
bool isLegalMove(Board *board, Move move)
{
    Piece piece = getPiece(board, move.fromRow, move.fromCol);

    // Cannot move an empty square
    if (piece.pieceType == EMPTY)
    {
        return false;
    }

    // Cannot move the opponent's piece
    if (piece.color != board->currentTurn)
    {
        return false;
    }

    // Must be reachable according to the piece's movement pattern
    if (!isPseudoLegalMove(board, move))
    {
        return false;
    }

    // Cannot leave your own king in check
    if (isInCheckAfterMove(board, move))
    {
        return false;
    }

    return true;
}

// Generates only the legal moves for the piece at (row, col)
// First generates pseudo-legal moves, then filters them with isLegalMove
void legalMovesForPiece(Board *board, int row, int col, MoveList *legalMoves)
{
    Piece piece = getPiece(board, row, col);
    MoveList pseudo;

    // Start both lists empty
    pseudo.index = 0;
    legalMoves->index = 0;

    if (board->moveCount > 0)
    {
        bool isAntEating = board->isAntEating;
        // You can't move anything but that anteater if last move was anteating
        if (isAntEating && (board->history[board->moveCount - 1].toRow != row || board->history[board->moveCount - 1].toCol != col))
        {
            return;
        }
    }

    // If the square is empty, there are no moves
    if (piece.pieceType == EMPTY)
    {
        return;
    }

    // Generate all reachable moves for this piece
    possibleMoves(&piece, board, row, col, &pseudo);

    // Keep only the moves that pass full legality checking
    for (int i = 0; i < pseudo.index; i++)
    {
        if (isLegalMove(board, pseudo.list[i]))
        {
            legalMoves->list[legalMoves->index] = pseudo.list[i];
            legalMoves->index++;
        }
    }
}

// Generates all legal moves for every piece of the specified color
// Temporarily changes currentTurn so isLegalMove works correctly
void allLegalMoves(Board *board, Color color, MoveList *legalMoves)
{
    Color originalTurn = board->currentTurn;
    board->currentTurn = color;

    // Start the final move list empty
    legalMoves->index = 0;

    // Search the whole board for pieces of the given color
    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            Piece p = getPiece(board, r, c);

            // Skip empty squares and opponent pieces
            if (p.pieceType == EMPTY || p.color != color)
            {
                continue;
            }

            // Generate legal moves for this one piece
            MoveList pieceMoves;
            pieceMoves.index = 0;

            legalMovesForPiece(board, r, c, &pieceMoves);

            // Append this piece's legal moves into the full list
            for (int i = 0; i < pieceMoves.index; i++)
            {
                legalMoves->list[legalMoves->index] = pieceMoves.list[i];
                legalMoves->index++;
            }
        }
    }

    // Restore the original turn after finishing
    board->currentTurn = originalTurn;
}

// Returns true if the given side has at least one legal move
bool hasAnyLegalMoves(Board *board, Color color)
{
    MoveList allMoves;
    allMoves.index = 0;

    allLegalMoves(board, color, &allMoves);

    return (allMoves.index > 0);
}

// A side is in checkmate if:
// 1. its king is in check
// 2. it has no legal moves
bool isCheckmate(Board *board, Color color)
{
    return isInCheck(board, color) && !hasAnyLegalMoves(board, color);
}

// A side is in stalemate if:
// 1. its king is NOT in check
// 2. it has no legal moves
bool isStalemate(Board *board, Color color)
{
    return !isInCheck(board, color) && !hasAnyLegalMoves(board, color);
}