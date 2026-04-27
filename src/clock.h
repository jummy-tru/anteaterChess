//header file for clock
#ifndef CLOCK_H
#define CLOCK_H
#include <stdbool.h>

typedef struct
{
	int elapsed_seconds;
	bool warning_shown;
} Timer;

void init_timer(Timer *t);
void increment_timer(Timer *t);
int get_time_elapsed(Timer *t);
bool get_warning_status(Timer *t);

#endif
