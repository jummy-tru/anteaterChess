#ifndef GUI_H
#define GUI_H

void set_controller(GameController *c);
void set_opponent_type(OpponentType *opponent);
void set_player_color(Color *player_color);
int run_gui(int argc, char *argv[]);
void log_move_to_sidebar(Piece p, int fromR, int fromC, int toR, int toC);

#endif