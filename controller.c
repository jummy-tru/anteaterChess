// File in charge of control flow (game start, menuing, quit game, etc)
#include "controller.h"
#include "clock.h"

void gameStart()
{
  init_clock(60);

  while (!is_time_up()) {
    update_clock();
}

void gameEnd()
{
  
}
gameEnd();
