#ifndef TIMER_H
#define TIMER_H

#include <time.h>

void timer_tick();
void timer_init();
time_t timer_time();
struct tm *timer_localtime();

#endif /* TIMER_H */
