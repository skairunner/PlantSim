#pragma once
#include <noise.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/normal_distribution.hpp>
#include <vector>
#include "Months.h"

namespace ALMANAC
  {
  using namespace std;

  
  class Weather
    {
    public:
    Weather(const vector<int>& rainyDaysPerMonth, const bool random = false);
    void step(const float& timestep);
    bool rain();

    private:
    boost::random::mt19937 gen;
    vector<int> wetDaysPerMonth; // <month, number of wet days>
    bool rainedYesterday;
    bool rainedToday;
    float rainedHowMuch;
    float rainCoef;

    Month currentMonth;

    void checkForRain();
    void setRainAmount();
    int random(const int& min, const int& max);
    double random(const double& min, const double& max);
    double normalRandom(); // Returns a normally distributed random number. mean 0 std 1
    ///
    //// Weather functions.
    ///
    };
  }