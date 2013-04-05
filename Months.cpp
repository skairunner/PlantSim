#include "Months.h"

namespace ALMANAC
  {
  using namespace std;
  std::string getMonthName(const int& MONTH)
    {
    //std::string output;
    if (MONTH <= 0 || MONTH > 13)
      return string("Month error");
    else
      switch(MONTH)
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

  Month::Month()
    :MONTH(JANUARY), date(1)
    {

    }

  Month::Month(const int& NEWMONTH, const int& day)
    {
    if (NEWMONTH > 0 && NEWMONTH < 14)
      MONTH = NEWMONTH;
    else
      MONTH = JANUARY;

    if (day > 0 && day < 31)
      {
      if (MONTH == FEBRUARY)
        {
        if (day < 29)
          date = day;
        }
      else
        date = day;
      }
    }

  void Month::advanceDay(const int& numDays)
    {
    date++;
    // check for feb
    if (MONTH == FEBRUARY){
      if (date > 28)
        {
        MONTH++;
        date = 1;
        }}
    else
      {
      if (MONTH != DECEMBER)
        { // First check for non-December cases.
        if (date > 30)
          {
          date = 1;
          MONTH++;
          }
        }
      else
        if (date > 30) // If it's both december and the date is over 30...
          {
          date = 1;
          MONTH = JANUARY;
          }
      }
    }

  void Month::setDate(const int& newDate)
    {
    if (date > 30 || date <= 0)
      {
      if (MONTH == FEBRUARY){
        if (newDate < 29)
          date = newDate;}
      else
        date = newDate;
      }
    }

  void Month::setMonth(const int& NEWMONTH)
    {
    if (NEWMONTH >= 1 && NEWMONTH < 13)
      MONTH = NEWMONTH;
    }
  
  int Month::getMonth()
    {
    return MONTH;
    }

  int Month::getDate()
    {
    return date;
    }

  int Month::getNumberOfDaysInMonth()
    {
    if (MONTH == FEBRUARY)
      return 28;
    else
      return 30;
    }

  }
