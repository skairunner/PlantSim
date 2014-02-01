#include "Months.h"

namespace ALMANAC
{
    using namespace std;
    //std::string getMonthName(const int MONTH)


    Month::Month()
        :MONTH(JANUARY), date(1), year(2013)
    {

    }

    Month::Month(const int& NEWMONTH, const int& day, const int& startYear)
    {
        year = startYear;
        if (NEWMONTH > 0 && NEWMONTH < 14)
            MONTH = NEWMONTH;
        else
        {
            MONTH = JANUARY;
            year++;
        }

        date = day;
    }

    ostream& operator<< (ostream& os, const Month& month)
    {
        os << getMonthName(month.getMonth()) << " " << month.getDate() << " " << month.getYear();
        return os;
    }

    void Month::advanceDay(const int& numDays)
    {
        int days = numDays;

        // Add days~
        for (; days > 0; days--)
        {
            date++;

            if (date == 31 && MONTH == DECEMBER)
                date = date;

            if (MONTH != DECEMBER) // First check for non-December cases.
            {
                if (date > 30)
                {
                    date = 1;
                    MONTH++;
                }
            }            

            else if (date > 30) // If it's both december and the date is over 30...
            {
                date = 1;
                MONTH = JANUARY;
                year++;
            }
        }

    }

    void Month::setDate(const int& newDate)
    {
        if (newDate < 31 && newDate > 0)
            date = newDate;
    }

    void Month::setMonth(const int& NEWMONTH)
    {
        if (NEWMONTH >= 1 && NEWMONTH < 13)
            MONTH = NEWMONTH;
    }

    int Month::getMonth() const
    {
        return MONTH;
    }

    int Month::getDate() const
    {
        return date;
    }

    int Month::getYear() const
    {
        return year;
    }

    int Month::getNumberOfDaysInMonth() const
    {
        /*   if (MONTH == FEBRUARY)
             return 28;
             else*/
        return 30;
    }

    int Month::getDaysSinceYearStart() const
    {
        int days = 0;
        int months = MONTH;

        months -= 1;

        days = months * 30 + date;

        return days;
    }

    int operator-(const Month& left, const Month& right)
    {
        int yearDiff = left.year - right.year;
        int monthDiff = left.MONTH - right.MONTH;
        int dayDiff = left.date - right.date;

        return yearDiff * 360 + monthDiff * 30 + dayDiff;
    }

    bool operator==(const Month& left, const Month& right)
    {
        return ((left.year == right.year) && (left.MONTH == right.MONTH) && (left.date == right.date));
    }
}

