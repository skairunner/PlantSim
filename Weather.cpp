#include "Weather.h"
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>


namespace ALMANAC
  {
  using namespace std;

  int Weather::random(const int& min, const int& max)
    {
    boost::random::uniform_int_distribution<int> distribution(min,max);
    return distribution(gen);
    }

  double Weather::random(const double& min, const double& max)
    {
    boost::random::uniform_real_distribution<double> distribution(min,max);
    return distribution(gen);
    }

  Weather::Weather(const vector<int>& rainyDaysPerMonth, const bool random):
    rainCoef(0.70f)
      {
      wetDaysPerMonth = rainyDaysPerMonth;
      }


    void Weather::step(const float& timestep)
      {
      checkForRain();
      setRainAmount();
      }

    void Weather::checkForRain()
      {
      currentMonth.advanceDay();
      float raw_chanceOfRain = (float)wetDaysPerMonth[currentMonth.getMonth()] / currentMonth.getNumberOfDaysInMonth();

      ///// If the previous day was wet...
      float chanceOfWetDry = raw_chanceOfRain * rainCoef;
      float chanceOfRain;
      if (rainedYesterday)
        chanceOfRain = 1.0f - rainCoef + chanceOfWetDry;
      else
        chanceOfRain = chanceOfWetDry;

      rainedYesterday = rainedToday;

      if (chanceOfRain >= random(0.0f, 1.0f)) // If the chance of rain > a random number, rain happens.
        rainedToday = true;
      else
        rainedToday = false;
      }

    void Weather::setRainAmount()
      {
      if (rainedToday)
        ;
      }

  }