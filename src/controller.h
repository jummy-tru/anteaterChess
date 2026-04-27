#ifndef CONTROLLER_H
#define CONTROLLER_H
#include "board.h"
#include "clock.h"
#include <stdbool.h>

typedef struct {
    Board board;
    bool square_selected;
    bool show_end_turn;
    int sel_row;
    int sel_col;
    // legal moves for currently selected piece
    MoveList legal_moves;
    Timer timer;
} GameController;

// opponent type and color selection
typedef enum { OPPONENT_HUMAN, OPPONENT_COMPUTER } OpponentType;

void select_square(GameController *c, int row, int col);
void clear_selection(GameController *c);
bool is_square_selected(GameController *c);
bool is_hint_square(GameController *c, int row, int col);
Piece get_selected_piece(GameController *c);
Piece controller_get_piece_at(GameController *c, int row, int col);
int get_selected_row(GameController *c);
int get_selected_col(GameController *c);

void process_end_turn(GameController *c);

Move get_played_move(int row, int col, GameController *c);
int is_legal_target(int row, int col, GameController *c);
int controller_get_move_count(GameController *c);

bool controller_in_checkmate(GameController *c);
bool controller_in_stalemate(GameController *c);

void update_anteating(GameController *c, bool b);
bool controller_in_anteating(GameController *c);

void controller_increment_timer(GameController *c);
int controller_get_time_elapsed(GameController *c);
bool controller_get_warning_status(GameController *c);

void switch_turn(GameController *c);
Color get_current_turn(GameController *c);

#endif
