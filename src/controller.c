// File in charge of control flow (game start, menuing, quit game, etc)

#include <stdio.h>
#include "controller.h"
#include "pieces.h"
#include "board.h"
#include "rules.h"
#include "gui.h"

void set_first_turn(GameController *c, Color first_turn)
{
    c->first_turn = first_turn;
}

// Controller initial setup
void init_controller(GameController *c)
{
    setupBoard(&c->board, c->first_turn);
    c->legal_moves.index = 0;
    init_timer(&c->timer);
    c->square_selected = false;
    c->show_end_turn = false;
    c->sel_col = -1;
    c->sel_row = -1;
}

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

// Returns true if a valid turn was made, returns false if not
bool process_cell_click(GameController *controller, int row, int col)
{
    Piece clicked = controller_get_piece_at(controller, row, col);
    int own = isOwnPiece(clicked, get_current_turn(controller));

    if (!is_square_selected(controller))
    {
        if (own)
        {
            select_square(controller, row, col);
            return false;
        }
    }
    else
    {
        if (is_legal_target(row, col, controller))
        {
            Move playedMove = get_played_move(row, col, controller);
            Piece selected = get_selected_piece(controller);
            log_move_to_sidebar(selected, get_selected_row(controller), get_selected_col(controller), row, col);
            Piece target = controller_get_piece_at(controller, row, col);
            
            if (selected.pieceType == ANTEATER && target.pieceType == PAWN)
            {
                update_anteating(controller, true);
                controller->show_end_turn = true;
            }
            applyMove(&controller->board, playedMove);

            if (controller_in_anteating(controller) == false)
            {
				switch_turn(controller);
                clear_selection(controller);
                return true;
            }
            else
            {
                clear_selection(controller);
                legalMovesForPiece(&controller->board, row, col, &controller->legal_moves);
                if (controller->legal_moves.index == 0)
                {
                    controller->board.isAntEating = false;
                    controller->show_end_turn = false;
                    switch_turn(controller);
                    return true;
                }
                return true;
            }
        }
        else if (own)
        {
            select_square(controller, row, col);
            controller->legal_moves.index = 0;
            legalMovesForPiece(&controller->board, row, col, &controller->legal_moves);
            return false;
        }
        else
        {
            clear_selection(controller);
            controller->legal_moves.index = 0;
            return false;
        }
    }
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