#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <gtk/gtk.h>

#include "pieces.h"
#include "board.h"
#include "moves.h"
#include "controller.h"
#include "gui.h"
#include "bots.h"
#include "main_funct.h"

typedef struct {
  GameController controller;
  OpponentType opponent;
  Color player_color;
} GameType;

static GameType game_type;

// Game initial setup
void init_game(GameController *c)
{
    setupBoard(&c->board);
    c->legal_moves.index = 0;
    init_timer(&c->timer);
    c->square_selected = false;
    c->show_end_turn = false;
    c->sel_col = -1;
    c->sel_row = -1;

    if (game_type.player_color == BLACK && game_type.opponent == OPPONENT_COMPUTER)
    { 
      Move bot_move = randomMove(c);
      applyMove(&c->board, bot_move);
      switch_turn(c);
    }
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
      int fromR = get_selected_row(controller);
      int fromC = get_selected_col(controller);
      
      applyMove(&controller->board, playedMove);

      Piece finalPiece = controller_get_piece_at(controller, row, col);
      log_move_to_sidebar(finalPiece, fromR, fromC, row, col);

      if (finalPiece.pieceType == ANTEATER && controller_in_anteating(controller))
      {
          controller->show_end_turn = true;
      }

      if (controller_in_anteating(controller) == false)
      {
        switch_turn(controller);
        clear_selection(controller);
        play_bot_turns(controller); 
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
          play_bot_turns(controller);
          return true;
        }
        return true;
      }
    }
    else if (own)
    {
      select_square(controller, row, col);
      return false;
    }
    else
    {
      clear_selection(controller);
      return false;
    }
  }
  return false;
}

void play_bot_turns(GameController *c)
{
  if (game_type.opponent == OPPONENT_HUMAN)
  {
    return;
  }
  if (c->board.currentTurn != game_type.player_color)
  {
    Move bot_move = randomMove(c);
    Piece selected = controller_get_piece_at(c, bot_move.fromRow, bot_move.fromCol);
    Piece target = controller_get_piece_at(c, bot_move.toRow, bot_move.toCol);
    
    if (selected.pieceType == ANTEATER && target.pieceType == PAWN)
    {
      log_move_to_sidebar(selected, get_selected_row(c), get_selected_col(c), bot_move.toRow, bot_move.toCol);
      update_anteating(c, true);
      applyMove(&c->board, bot_move);
    }
    else
    {
      log_move_to_sidebar(selected, get_selected_row(c), get_selected_col(c), bot_move.toRow, bot_move.toCol);
      applyMove(&c->board, bot_move);
      switch_turn(c);
    }
  }
}

int main(int argc, char *argv[])
{
  srand(time(NULL));

  set_controller(&game_type.controller);
  set_opponent_type(&game_type.opponent);
  set_player_color(&game_type.player_color);
  run_gui(argc, argv);
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
}