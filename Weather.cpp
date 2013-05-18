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

  double Weather::getMaxTemp()
    {
    return maxTemp;
    }

  double Weather::getMinTemp()
    {
    return minTemp;
    }

  double Weather::getDayRadiation()
    {
    return dayRadiation;
    }

  Month Weather::getMonth()
    {
    return currentMonth;
    }

  void Weather::changeDate(const int month, const int date)
    {
    currentMonth.setMonth(month);
    currentMonth.setDate(date);
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

  double Weather::normalRandom(const double& mean, const double& stdev)
    {
    boost::random::normal_distribution<> dist(mean, stdev);
    return dist(gen);
    }

  Weather::Weather(const vector<int>& rainyDaysPerMonth, const bool random):                                                                                              
    rainCoef(0.70f), rainedHowMuch(0.0f), omegaT(0.5f), omegaR(0.5f), defaultRadiation(16.056f)
      {
      wetDaysPerMonth = rainyDaysPerMonth;

      gen.seed(rand());      
      }

    bool Weather::loadRain(const vector<vector<double>>& rainMeans, const vector<vector<double>>& rainStats)
      {
      Parse::StatisticsParser statsparser;
      RainHolder.push_back(StatsHolder());
      int month = 1;
      for (int it = 0; it < rainStats.size(); it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(rainStats[it]); // Load the data into the stats parser to get statistical numbers.
        RainHolder.push_back(StatsHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), rainMeans[it]));
        // In the above case, the same int is plugged into the StatsHolder. Normally, the *it for statsparser would be a vector<vector<float>>,
        // as many months of stats, while the *it for StatsHolder is the 30 year mean.
        month++;
        }
      loadedRain = true;
      return true;
      }

    bool Weather::loadTemps(const vector<vector<double>>& tempHiMeans, const vector<vector<double>>& tempLowMeans, const vector<vector<double>>& tempHiStats, const vector<vector<double>>& tempLowStats)
      {
      Parse::StatisticsParser statsparser;
      MaxTemp.push_back(StatsHolder());
      MinTemp.push_back(StatsHolder());

      int month = 1;
      for (int it = 0; it < tempHiMeans.size() - 1; it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(tempHiStats[it]); // Load the data into the stats parser to get statistical numbers.
        MaxTemp.push_back(StatsHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), tempHiMeans[it]));
       
        statsparser.loadData(tempLowStats[it]);
        MinTemp.push_back(StatsHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), tempLowMeans[it]));
        month++;
        }

      loadedTemp = true;
      return true;
      }

    bool Weather::loadSun(const vector<vector<double>>& sunlightMean)
      {
      Parse::StatisticsParser statsparser;
      sunlight.push_back(StatsHolder());

      int month = 1;
      for (int it = 0; it < sunlightMean.size() - 1; it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(sunlightMean[it]); // Load the data into the stats parser to get statistical numbers.
        sunlight.push_back(StatsHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), sunlightMean[it]));

        month++;
        }

      loadedSun = true;
      return true;
      }

    bool Weather::loadSTDEVs(const vector<vector<double>>& STDEVs)
      {
      for (int counter = 1; counter <= 12; counter++)
        {
        RainHolder[counter].standardDeviation = STDEVs[counter].at(Parse::stat::RAIN);
        MaxTemp[counter].standardDeviation = STDEVs[counter].at(Parse::stat::TEMPMAX);
        MinTemp[counter].standardDeviation = STDEVs[counter].at(Parse::stat::TEMPMIN);
        }

      return true;
      }

    void Weather::step(const float& timestep)
      {
      if (loadedRain && loadedTemp && loadedSun)
        {
        checkForRain();
        setRainAmount();
        findTemp();
        findRadiation();
        }
      else
        cerr << "Loaded rain: " << loadedRain << "\n Loaded temperatures: " << loadedTemp << "\n Loaded sunlight: " << loadedSun << endl;
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
        double standardDev = RainHolder[currentmonth].standardDeviation;
        double skew = RainHolder[currentmonth].skew;
        double dailyMean = RainHolder[currentmonth].means[currentMonth.getDate()];
        double randomn = normalRandom();
        rainedHowMuch = (pow(((randomn - skew / 6.0f) * (skew / 6.0f) + 1.0f), 3) - 1.0f) * standardDev / skew + dailyMean * 2.0f; // changed the *2.0f from the front to the back.
        rainedHowMuch = rainedHowMuch > 0.01 ? rainedHowMuch : 0.01;
        }
      else
        rainedHowMuch = 0.0f;
      }

    void Weather::findTemp()
      {
      double dayMaxTemp = MaxTemp[currentMonth.getMonth()].means[currentMonth.getDate()];
      double dayMinTemp = MinTemp[currentMonth.getMonth()].means[currentMonth.getDate()];
      double possibleMaxTemp = dayMaxTemp + (wetDaysPerMonth[currentMonth.getMonth()] / currentMonth.getNumberOfDaysInMonth()) * omegaT * (dayMaxTemp - dayMinTemp);
      double wetMaxTemp = possibleMaxTemp - omegaT * (dayMaxTemp - dayMinTemp);
      double possibleMinTemp = dayMinTemp + (wetDaysPerMonth[currentMonth.getMonth()] / currentMonth.getNumberOfDaysInMonth()) * omegaT * (dayMaxTemp - dayMinTemp);
      if (rainedToday)
        maxTemp = normalRandom(wetMaxTemp, MaxTemp[currentMonth.getMonth()].standardDeviation);
      else
        maxTemp = normalRandom(possibleMaxTemp, MaxTemp[currentMonth.getMonth()].standardDeviation);

      minTemp = normalRandom(possibleMinTemp, MinTemp[currentMonth.getMonth()].standardDeviation/3);

      // swap max and min if min > max
      if (minTemp > maxTemp)
        {
        double temp = maxTemp;
        maxTemp = minTemp;
        minTemp = maxTemp;
        }
      }

    void Weather::findRadiation()
      {
      double baseRad = sunlight[currentMonth.getMonth()].means[currentMonth.getDate()] * defaultRadiation;
      double dryRad = (baseRad * currentMonth.getNumberOfDaysInMonth()) / (omegaR * wetDaysPerMonth[currentMonth.getMonth()] + (currentMonth.getNumberOfDaysInMonth() - wetDaysPerMonth[currentMonth.getMonth()]));
      double wetRad = dryRad * omegaR;

      if (rainedToday)
        dayRadiation = normalRandom(wetRad, sunlight[currentMonth.getMonth()].standardDeviation);
      else
        dayRadiation = normalRandom(dryRad, sunlight[currentMonth.getMonth()].standardDeviation);
      }
  }