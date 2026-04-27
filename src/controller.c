// File in charge of control flow (game start, menuing, quit game, etc)

#include <stdio.h>
#include "controller.h"
#include "pieces.h"
#include "board.h"
#include "rules.h"
#include "gui.h"

void select_square(GameController *c, int row, int col)
{
    c->square_selected = true;
    c->sel_row = row;
    c->sel_col = col;
    c->legal_moves.index = 0;
    legalMovesForPiece(&c->board, row, col, &c->legal_moves);
}

void clear_selection(GameController *c)
{
    c->square_selected = false;
    c->legal_moves.index = 0;
}

bool is_square_selected(GameController *c)
{
    return c->square_selected;
}

bool is_hint_square(GameController *c, int row, int col)
{
    for (int i = 0; i < c->legal_moves.index; i++)
    {
        if (c->legal_moves.list[i].toRow == row && c->legal_moves.list[i].toCol == col)
        {
            return true;
        }
    }
    return false;
}

Piece get_selected_piece(GameController *c)
{
    return getPiece(&c->board, c->sel_row, c->sel_col);
}

Piece controller_get_piece_at(GameController *c, int row, int col)
{
    return getPiece(&c->board, row, col);
}

int get_selected_row(GameController *c)
{
    return c->sel_row;
}

int get_selected_col(GameController *c)
{
    return c->sel_col;
}

void process_end_turn(GameController *c)
{
    c->board.isAntEating = false;
    switch_turn(c);
}

// Only use when move is known to be legal
Move get_played_move(int row, int col, GameController *c)
{
    for (int i = 0; i < c->legal_moves.index; i++)
        if (c->legal_moves.list[i].toRow == row && c->legal_moves.list[i].toCol == col)
            return c->legal_moves.list[i];
    
    return createMove(-1, -1, -1, -1);
}

// Check if piece can be moved to a new square legally
int is_legal_target(int row, int col, GameController *c)
{
    for (int i = 0; i < c->legal_moves.index; i++)
        if (c->legal_moves.list[i].toRow == row && c->legal_moves.list[i].toCol == col)
            return 1;
    return 0;
}

int controller_get_move_count(GameController *c)
{
    return c->board.moveCount;
}

bool controller_in_checkmate(GameController *c)
{
    return isCheckmate(&c->board, c->board.currentTurn);
}

bool controller_in_stalemate(GameController *c)
{
    return isStalemate(&c->board, c->board.currentTurn);
}

void update_anteating(GameController *c, bool b)
{
    c->board.isAntEating = b;
}

bool controller_in_anteating(GameController *c)
{
    return c->board.isAntEating;
}

void controller_increment_timer(GameController *c)
{
    increment_timer(&c->timer);
}

int controller_get_time_elapsed(GameController *c)
{
    return get_time_elapsed(&c->timer);
}

bool controller_get_warning_status(GameController *c)
{
    return get_warning_status(&c->timer);
}

// Change the the turn of the game to the other player
void switch_turn(GameController *c) 
{
    c->board.currentTurn = (c->board.currentTurn == WHITE) ? BLACK : WHITE;
    init_timer(&c->timer);
}

// Check whose turn it is
Color get_current_turn(GameController *c)
{
    return c->board.currentTurn;
}