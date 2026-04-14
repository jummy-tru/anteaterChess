// File in charge of control flow (game start, menuing, quit game, etc)

#include <stdio.h>
#include "controller.h"
#include "clock.h"

void showMenu() {
  printf("=== GAME START ===\n");
  printf("Select Mode:\n");
  printf("1. Human vs Human\n");
  printf("2. Human vs Robot\n");
  printf("3. Quit\n);
  printf("Enter choice: ");
}

void gameStart()
{
    int mode;

  showMenu();
  scanf("%d", &mode);

  if (mode == 3) {
    printf("Exiting game... \n");
    return;
  }

  printf("\nStarting game...\n");

  init_clock(60); // 60 second timer

    while (!is_time_up()) {
        update_clock();

        if (mode == 1) {
            printf("Human vs Human turn...\n");
        }
        else if (mode == 2) {
            printf("Human vs Robot turn...\n");
        }
    }
    gameEnd();
}

void gameEnd()
{
    printf("=== GAME OVER ===\n");

    if (is_time_up()) {
        printf("Time is up!\n");
    }

    printf("Thanks for playing!\n");
}
