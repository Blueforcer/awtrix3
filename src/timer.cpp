#include "timer.h"

time_t now;
struct tm *datetime;

time_t timer_time()
{
    return now;
}

struct tm *timer_localtime()
{
    return datetime;
}

void timer_tick()
{
    now = time(NULL);
    datetime = localtime(&now);
}
