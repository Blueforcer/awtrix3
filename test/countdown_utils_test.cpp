#include "../src/CountdownUtils.h"

#include <cassert>

int main()
{
    assert(countdownDisplayState(1) == CountdownDisplayState::Active);
    assert(countdownDisplayState(0) == CountdownDisplayState::DueToday);
    assert(countdownDisplayState(-1) == CountdownDisplayState::Expired);

    CountdownDate target;
    assert(parseCountdownDate("2026-06-05", target));
    assert(countdownDaysBetweenDates(2026, 6, 5, target.year, target.month, target.day) == 0);
    assert(countdownDaysBetweenDates(2026, 6, 4, target.year, target.month, target.day) == 1);
    assert(countdownDaysBetweenDates(2026, 6, 6, target.year, target.month, target.day) == -1);

    return 0;
}
