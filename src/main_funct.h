#ifndef MAIN_FUNCT_H
#define MAIN_FUNCT_H
#include "controller.h"

void init_game(GameController *c);
bool process_cell_click(GameController *controller, int row, int col);
void play_bot_turns(GameController *c);

#endif