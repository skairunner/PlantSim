#pragma once
#include "enums.h"
#include <utility>
#include <string>

namespace ALMANAC
  {
  static std::string getMonthName(const int& MONTH);

  class Month
    {
    public:
      Month();
      Month(const int& NEWMONTH, const int& day);
      void advanceDay(const int& numDays = 1);
      void setDate(const int& newDate);
      void setMonth(const int& NEWMONTH);
      std::pair<int, int> getFullDate(); // Gets both the date and the month.
      int getMonth();
      int getDate();
      int getNumberOfDaysInMonth();
      

    private:
      int MONTH;
      int date;
    };
  }