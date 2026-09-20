#include "CountdownUtils.h"

#include <stdlib.h>

static bool isLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static int daysInMonth(int year, int month)
{
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }
    return days[month - 1];
}

static bool isValidDate(int year, int month, int day)
{
    return year >= 1970 && month >= 1 && month <= 12 && day >= 1 && day <= daysInMonth(year, month);
}

static long daysFromCivil(int year, unsigned month, unsigned day)
{
    year -= month <= 2;
    const long era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(year - era * 400);
    const unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<long>(doe) - 719468;
}

bool parseCountdownDate(const char *value, CountdownDate &date)
{
    if (value == nullptr)
    {
        return false;
    }

    for (int i = 0; i < 10; ++i)
    {
        if ((i == 4 || i == 7) ? value[i] != '-' : value[i] < '0' || value[i] > '9')
        {
            return false;
        }
    }

    if (value[10] != '\0')
    {
        return false;
    }

    int year = atoi(value);
    int month = atoi(value + 5);
    int day = atoi(value + 8);

    if (!isValidDate(year, month, day))
    {
        return false;
    }

    date.year = year;
    date.month = month;
    date.day = day;
    return true;
}

long countdownDaysBetweenDates(int currentYear, int currentMonth, int currentDay, int targetYear, int targetMonth, int targetDay)
{
    if (!isValidDate(currentYear, currentMonth, currentDay) || !isValidDate(targetYear, targetMonth, targetDay))
    {
        return 0;
    }

    return daysFromCivil(targetYear, targetMonth, targetDay) - daysFromCivil(currentYear, currentMonth, currentDay);
}

CountdownDisplayState countdownDisplayState(long daysRemaining)
{
    if (daysRemaining < 0)
    {
        return CountdownDisplayState::Expired;
    }

    if (daysRemaining == 0)
    {
        return CountdownDisplayState::DueToday;
    }

    return CountdownDisplayState::Active;
}
