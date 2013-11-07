#include "Weather.h"
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include "parser_rain.h"
#include <boost/math/distributions/inverse_gaussian.hpp>
#include <stdlib.h>

namespace ALMANAC
  {
  using namespace std;

  WeatherData::WeatherData(){}

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

  double Weather::getAverageHumidity()
    {
    return humidity;
    }

  WeatherData Weather::getDataBundle()
    {
    WeatherData wd;
    wd.minTemp = getMinTemp();
    wd.maxTemp = getMaxTemp();
    wd.humidity = getAverageHumidity();
    wd.radiation = getDayRadiation();
    wd.CO2 = 440;
    wd.meanWindSpeed = 0;

    return wd;
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
    rainCoef(0.70f), rainedHowMuch(0.0f), omegaT(0.5f), omegaR(0.5f), defaultRadiation(445.0f), omegaH(0.9f)
      {
      wetDaysPerMonth = rainyDaysPerMonth;

      gen.seed(rand());      
      }

    Weather::Weather(bool def)
      {
      Parse::MonolithParse parser(std::string("Content/seosan_skew.monolith"));
      //parser.load();
      auto skewInfo = parser.parse();

      Parse::MonolithParse parseStdev(std::string("Content/seosan_stdev.monolith"));
      parseStdev.load();
      auto STDEVinfo = parseStdev.parse();

      Parse::Parser_RainyDays rainyDaysP("Content/unknownRainyDays.rain");
      rainyDaysP.parse();
      auto result = rainyDaysP.getResult();

      Parse::Parser rainMeansP("Content/30yearaverage.rain");
      auto rainMeans = rainMeansP.parse();

      Parse::Parser sunP("Content/30YearAverage.sun");
      auto sunMeans = sunP.parse();

      Parse::Parser TempHiP("Content/30YearAverage.temphi");
      auto TempHiInfo = TempHiP.parse();

      Parse::Parser TempLoP("Content/30YearAverage.templow");
      auto TempLoInfo = TempLoP.parse();

      Parse::Parser HumidityP("Content/30YearAverage.humid");
      auto HumidityInfo = HumidityP.parse();

      *this = Weather(result, false);

      loadRain(rainMeans, STDEVinfo, skewInfo);
      loadSun(sunMeans);
      loadTemps(TempHiInfo, TempLoInfo, STDEVinfo);
      loadHumidity(HumidityInfo);
      }

    bool Weather::loadRain(const vector<vector<double>>& rainMeans, const vector<vector<double>>& stdevs, const vector<vector<double>>& skews)
      {
      Parse::StatisticsParser statsparser;
      RainHolder.push_back(StatsHolder());
      int month = 1;
      for (int it = 0; it < rainMeans.size(); it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(rainMeans[it]); // Monthly mean.
        RainHolder.push_back(StatsHolder(month, statsparser.getMean(), stdevs.at(month).at(4), skews.at(month).at(1), rainMeans[it]));
        // In the above case, the same int is plugged into the StatsHolder. Normally, the *it for statsparser would be a vector<vector<float>>,
        // as many months of stats, while the *it for StatsHolder is the 30 year mean.
        month++;
        }
      loadedRain = true;
      return true;
      }

    bool Weather::loadTemps(const vector<vector<double>>& tempHiMeans, const vector<vector<double>>& tempLowMeans, const vector<vector<double>>& STDEVs)
      {
      Parse::StatisticsParser statsparser;
      MaxTemp.push_back(StatsHolder());
      MinTemp.push_back(StatsHolder());

      int month = 1;
      for (int it = 0; it < tempHiMeans.size() - 1; it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(tempHiMeans[it]); // Load the data into the stats parser to get the means.
        MaxTemp.push_back(StatsHolder(month, statsparser.getMean(), STDEVs.at(month).at(2), 0, tempHiMeans[it]));
       
        statsparser.loadData(tempLowMeans[it]);
        MinTemp.push_back(StatsHolder(month, statsparser.getMean(), STDEVs.at(month).at(3), 0, tempLowMeans[it]));
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

    bool Weather::loadHumidity(const vector<vector<double>>& Humidness)
      {
      Parse::StatisticsParser statsparser;
      MeanHumidity.push_back(StatsHolder());

      int month = 1;
      for (int it = 0; it < Humidness.size() - 1; it++)
        // Skip the first one, which is a 0.
        {
        statsparser.loadData(Humidness[it]); // Load the data into the stats parser to get statistical numbers.
        MeanHumidity.push_back(StatsHolder(month, statsparser.getMean(), statsparser.getSTDEV(), statsparser.getSkew(), Humidness[it]));

        month++;
        }

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

    bool Weather::loadSkew(const vector<vector<double>>& skewVec)
      {
      for (int counter = 0; counter < 12; counter++)
        {
        RainHolder.at(counter+1).skew = skewVec.at(counter).at(1);
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
        findHumidity();
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
      double possibleMaxTemp = dayMaxTemp + (wetDaysPerMonth[currentMonth.getMonth()] / (double)currentMonth.getNumberOfDaysInMonth()) * omegaT * (dayMaxTemp - dayMinTemp);
      double wetMaxTemp = possibleMaxTemp - omegaT * (dayMaxTemp - dayMinTemp);
      double possibleMinTemp = dayMinTemp + (wetDaysPerMonth[currentMonth.getMonth()] / (double)currentMonth.getNumberOfDaysInMonth()) * omegaT * (dayMaxTemp - dayMinTemp);
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
      double baseRad = sunlight[currentMonth.getMonth()].means[currentMonth.getDate()] * defaultRadiation * 7.3; // In this case, 7 hours of daylight is assumed. 
      // Have to add a way to make the hous per day vary
      double dryRad = (baseRad * currentMonth.getNumberOfDaysInMonth()) / (omegaR * wetDaysPerMonth[currentMonth.getMonth()] + (currentMonth.getNumberOfDaysInMonth() - wetDaysPerMonth[currentMonth.getMonth()]));
      double wetRad = dryRad * omegaR;

      if (rainedToday)
        dayRadiation = normalRandom(wetRad, sunlight[currentMonth.getMonth()].standardDeviation);
      else
        dayRadiation = normalRandom(dryRad, sunlight[currentMonth.getMonth()].standardDeviation);

      dayRadiation *= 0.0036; // To convert units into MJ/m^2
      }

    void Weather::findHumidity()
      {
      double meanHumidity = MeanHumidity[currentMonth.getMonth()].means[currentMonth.getDate()];
      double wetDayProb = omegaH * wetDaysPerMonth[currentMonth.getMonth()] / (double)currentMonth.getNumberOfDaysInMonth();
      double RHD = meanHumidity - (wetDayProb) / (1 - wetDayProb);
      double RHW = meanHumidity + omegaH * (1 - meanHumidity/100.0f);
      double triangularPeak = rainedToday ? RHW : RHD;
      double triangularUpper = triangularPeak + (1 - triangularPeak / 200.0f) * exp(triangularPeak/100.0f - 1);
      double triangularLower = triangularPeak * (1 - exp(-meanHumidity));
      double randNum = random(0, 1);
      humidity = triangularLower + sqrt(randNum * (triangularPeak - triangularLower) * (triangularUpper - triangularLower));
      }
  }