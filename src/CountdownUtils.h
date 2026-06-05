#ifndef CountdownUtils_h
#define CountdownUtils_h

struct CountdownDate
{
    int year;
    int month;
    int day;
};

enum class CountdownDisplayState
{
    Active,
    DueToday,
    Expired
};

bool parseCountdownDate(const char *value, CountdownDate &date);
long countdownDaysBetweenDates(int currentYear, int currentMonth, int currentDay, int targetYear, int targetMonth, int targetDay);
CountdownDisplayState countdownDisplayState(long daysRemaining);

#endif
