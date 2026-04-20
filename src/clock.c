// implementing clock
#include "clock.h"

static int time_left;

void init_clock(int seconds) {
    time_left = seconds;
}

void update_clock() {
    if (time_left > 0) {
        time_left--;
    }
}

int get_time_left() {
    return time_left;
}

int is_time_up() {
    return time_left <= 0;
}
