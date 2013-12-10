#pragma once
#include "enums.h"
#include <utility>
#include <string>
#include <ostream>

namespace ALMANAC
{
    using namespace std;

    static std::string getMonthName(const int MONTH)
    {
        //std::string output;
        if (MONTH <= 0 || MONTH > 13)
            return string("Month error");
        else
            switch (MONTH)
        {
            case JANUARY: return string("January"); break;
            case FEBRUARY: return string("February"); break;
            case MARCH: return string("March"); break;
            case APRIL: return string("April"); break;
            case MAY: return string("May"); break;
            case JUNE: return string("June"); break;
            case JULY: return string("July"); break;
            case AUGUST: return string("August"); break;
            case SEPTEMBER: return string("September"); break;
            case OCTOBER: return string("October"); break;
            case NOVEMBER: return string("November"); break;
            case DECEMBER: return string("December"); break;
            default: return string("Switch error"); break;
        }
    }

    class Month
    {
    public:
        Month();
        Month(const int& NEWMONTH, const int& day, const int& startYear = 2013);
        void advanceDay(const int& numDays = 1);
        void setDate(const int& newDate);
        void setMonth(const int& NEWMONTH);
        std::pair<int, int> getFullDate(); // Gets both the date and the month.
        int getYear() const;
        int getMonth() const;
        int getDate() const;
        int getNumberOfDaysInMonth() const;
        int getDaysSinceYearStart() const;

        friend int operator- (const Month&, const Month&);

    private:
        int MONTH;
        int date;
        int year;
    };

    int operator-(const Month& left, const Month& right);
    ostream& operator<< (ostream& os, const Month& month);

}