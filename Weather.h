#pragma once
#include <noise.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>
#include "Months.h"
#include "rain_stats_holder.h"

namespace ALMANAC
  {
  using namespace std;

  
  class Weather
    {
    public:
    Weather(const vector<int>& rainyDaysPerMonth, const bool random = false);
    bool loadRain(const vector<vector<double>>& rainMeans, const vector<vector<double>>& rainStats);
    bool loadTemps(const vector<vector<double>>& tempHiMeans, const vector<vector<double>>& tempLowMeans, const vector<vector<double>>& tempHiStats, const vector<vector<double>>& tempLowStats);
    bool loadSun(const vector<vector<double>>& sunlightMean);

    void step(const float& timestep);
    bool rain();

    double getRainAmount();
    double getMaxTemp();
    double getMinTemp();
    double getDayRadiation();

    Month getMonth();
    void changeDate(const int month, const int date);

    //////////////////////////
    private:
    boost::random::mt19937 gen;
    vector<int> wetDaysPerMonth; // <month, number of wet days>

    bool loadedRain, loadedTemp, loadedSun;

    Month currentMonth;
    vector<StatsHolder> RainHolder;
    vector<StatsHolder> MaxTemp;
    vector<StatsHolder> MinTemp;
    vector<StatsHolder> sunlight;

    ///
    //// Rain
    ///
    void checkForRain();
    void setRainAmount();
    bool rainedYesterday, rainedToday;
    double rainedHowMuch;
    double rainCoef;
    ///
    //// Sun
    ///

    void findTemp();
    void findRadiation();
    double minTemp, maxTemp, dayRadiation;
    double directSun;
    double diffuseSun;
    double omegaT, // How much wet days affect temp. 0.5
           omegaR, // 0.5
           defaultRadiation; // MJ * m^2 (16.056 atm)

    ///
    //// RNGs
    ///
    int random(const int& min, const int& max);
    double random(const double& min, const double& max);
    double normalRandom(); // Returns a normally distributed random number. mean 0 std 1
    double normalRandom(const double& mean, const double& stdev); // Returns a normally distributed random number with specified mean & stdev
    };
  }