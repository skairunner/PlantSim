#include "Weather.h"
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "parser_rain.h"
#include <boost/math/distributions/inverse_gaussian.hpp>
#include <stdlib.h>

namespace ALMANAC
  {
  using namespace std;

  int Weather::random(const int& min, const int& max)
    {
    boost::random::uniform_int_distribution<int> distribution(min,max);
    return distribution(gen);
    }
  
  bool Weather::rain()
    {
    return rainedToday;
    }

  double Weather::getRainAmount()
    {
    return rainedHowMuch;
    }

  Month Weather::getMonth()
    {
    return currentMonth;
    }

  void Weather::changeDate(const int month, const int date)
    {
    currentMonth.setMonth(month);
    currentMonth.setDate(month);
    }

  double Weather::random(const double& min, const double& max)
    {
    boost::random::uniform_real_distribution<double> distribution(min,max);
    return distribution(gen);
    }

  double Weather::normalRandom()
    {
    boost::random::normal_distribution<> dist(0, 1);
    return dist(gen);
    }

  Weather::Weather(const vector<int>& rainyDaysPerMonth, const vector<vector<double>> rainMeans, const vector<vector<double>> rainStats, const bool random):
    //                                                                                                     ^ Each vector<vector<>> is one month, consolidated from 30 years.
    rainCoef(0.70f), rainedHowMuch(0.0f)
      {
      wetDaysPerMonth = rainyDaysPerMonth;
      Parse::RainStatisticsParser statsparser;
      gen.seed(rand());
      
      rainHolder.push_back(RainHolder());
      int month = 1;
      for (int it = 0; it < rainStats.size(); it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(rainStats[it]); // Load the data into the stats parser to get statistical numbers.
        rainHolder.push_back(RainHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), rainMeans[it]));
        // In the above case, the same int is plugged into the rainholder. Normally, the *it for statsparser would be a vector<vector<float>>,
        // as many months of stats, while the *it for rainHolder is the 30 year mean.
        month++;
        }
      
      }


    void Weather::step(const float& timestep)
      {
      checkForRain();
      setRainAmount();
      }

    void Weather::checkForRain()
      {
      currentMonth.advanceDay();
      double raw_chanceOfRain = (double)wetDaysPerMonth[currentMonth.getMonth()] / currentMonth.getNumberOfDaysInMonth();

      ///// If the previous day was wet...
      double chanceOfWetDry = raw_chanceOfRain * rainCoef;
      double chanceOfRain;
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
        {
        int currentmonth = currentMonth.getMonth();
        double standardDev = rainHolder[currentmonth].standardDeviation;
        double skew = rainHolder[currentmonth].skew;
        double dailyMean = rainHolder[currentmonth].means[currentMonth.getDate()];
        double randomn = normalRandom();
        rainedHowMuch = (pow(((randomn - skew / 6.0f) * (skew / 6.0f) + 1.0f), 3) - 1.0f) * standardDev / skew + dailyMean * 2.0f; // changed the *2.0f from the front to the back.
        rainedHowMuch = rainedHowMuch > 0.01 ? rainedHowMuch : 0.01;
        }
      else
        rainedHowMuch = 0.0f;
      }

  }