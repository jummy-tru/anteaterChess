// implementing clock
#include "clock.h"

void init_timer(Timer *t) {
    t->elapsed_seconds = 0;
    t->warning_shown = false;
}

void increment_timer(Timer *t) {
    t->elapsed_seconds++;
    if (t->elapsed_seconds >= 60)
    {
        t->warning_shown = true;
    }
}

int get_time_elapsed(Timer *t) {
    return t->elapsed_seconds;
}

bool get_warning_status(Timer *t)
{
    return t->warning_shown;
}
