#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <stdlib.h>

typedef struct timeval Timer;

static Timer get_timer() {
    Timer st;
    gettimeofday(&st, NULL);
    return st;
}

static void reset_timer(Timer *st) {
    gettimeofday(st, NULL);
}

static bool timer_running(Timer *st) {
    return !(st->tv_sec == 0 && st->tv_usec == 0);
}

static int get_lap_time_us(Timer *st) {
    if(!timer_running(st)) return 0;
    Timer et;
    gettimeofday(&et, NULL);
    int elapsed = ((et.tv_sec - st->tv_sec) * 1000000) + (et.tv_usec - st->tv_usec);
    return elapsed;
}

static void stop_timer(Timer *st) {
    st->tv_sec = 0; st->tv_usec = 0;
}


#endif