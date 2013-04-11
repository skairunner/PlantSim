#include <iostream>
#include "parser_rain.h"
#include "Weather.h"
#include <string>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

using namespace std;

int main()
  {
  Parse::Parser_RainyDays rainyDaysPerMonth(string("Content/unknownRainyDays.rain"));
  Parse::Parser rainStats(string("Content/30yearaverage.rain"));
  Parse::Parser tempHiStats (string("Content/30YearAverage.temphi"));
  Parse::Parser tempLowStats (string("Content/30YearAverage.templow"));
  Parse::Parser sunStats (string("Content/30YearAverage.sun"));
  Parse::outputColumn analysis;

  bool rainydayspermonthTrue = rainyDaysPerMonth.load();
  bool rainStatsTrue = rainStats.load();
  bool tempHiStatsTrue = tempHiStats.load();
  bool tempLowStatsTrue = tempLowStats.load();
  bool sunStatsTrue = sunStats.load();

  srand(time(0)); 

  if (rainydayspermonthTrue && rainStatsTrue && tempHiStatsTrue && tempLowStatsTrue)
    {
    rainyDaysPerMonth.parse();
    vector<vector<double>> rainstats = rainStats.parse();
    vector<vector<double>> temphistats = tempHiStats.parse();
    vector<vector<double>> templowstats = tempLowStats.parse();
    vector<vector<double>> sunlight = sunStats.parse();
    vector<double> his;
    vector<double> lows;

    ALMANAC::Weather WeatherModule(rainyDaysPerMonth.getResult(), sunlight, true);
    WeatherModule.loadRain(rainstats, rainstats);
    WeatherModule.loadTemps(temphistats, templowstats, temphistats, templowstats);
  //  WeatherModule.loadSun(sunlight);
    
    long double totalRain = 0;
    WeatherModule.changeDate(11, 1);

    for (int counter = 0; counter < 30; counter++)
      {
      WeatherModule.step(1);
      cout << WeatherModule.getMonth() << " : ";
      cout << "\n  High " << std::setprecision(2) << WeatherModule.getMaxTemp() << "¡ÆC\n   Low " << WeatherModule.getMinTemp() << "¡ÆC\n";
      his.push_back(WeatherModule.getMaxTemp());
      lows.push_back(WeatherModule.getMinTemp());
      }

    analysis.setfilename(string("hi.txt"));
    analysis.output(his);
    analysis.setfilename(string("lows.txt"));
    analysis.output(lows);
    }
  else
    cout << "Error loading file(s):\n    Rain stats=" << rainStatsTrue << "\n    Temp(hi) stats=" << tempHiStatsTrue
         << "\n    Temp(low) stats=" << tempLowStatsTrue << endl;
  cin.ignore(1);

  return 0;
  }