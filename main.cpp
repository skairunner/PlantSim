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
    vector<double> his, lows, rain, radiation;

    ALMANAC::Weather WeatherModule(rainyDaysPerMonth.getResult(), true);
    WeatherModule.loadRain(rainstats, rainstats);
    WeatherModule.loadTemps(temphistats, templowstats, temphistats, templowstats);
    WeatherModule.loadSun(sunlight);
    
    long double totalRain = 0;
   // WeatherModule.changeDate(7, 1);
    

    // Initialize files.
    analysis.setfilename(string("hi.txt"));
    analysis.clearFile();
    analysis.setfilename(string("lows.txt"));
    analysis.clearFile();
    analysis.setfilename(string("rain.txt"));
    analysis.clearFile();
    WeatherModule.changeDate(12, 30);
    int month = WeatherModule.getMonth().getMonth();

    for (int counter = 0; counter < 366; counter++)
      {
      WeatherModule.step(1);
      cout << WeatherModule.getMonth() << " : ";

      if ( month != WeatherModule.getMonth().getMonth())
        {
        month = WeatherModule.getMonth().getMonth();
        analysis.setfilename(string("hi.txt"));
        analysis.outputRow(his);
        analysis.setfilename(string("lows.txt"));
        analysis.outputRow(lows);
        analysis.setfilename(string("rain.txt"));
        analysis.outputRow(rain);
        his.clear();
        lows.clear();
        rain.clear();
        }

      cout << "\n  High " << std::setprecision(3) << WeatherModule.getMaxTemp() << "¡ÆC\n  Low " << WeatherModule.getMinTemp() << "¡ÆC\n" << "  Radiation: " << WeatherModule.getDayRadiation() << " MJ / m^2\n";
      cout << "  Rain " << WeatherModule.getRainAmount() << "mm\n";
      his.push_back(WeatherModule.getMaxTemp());
      lows.push_back(WeatherModule.getMinTemp());
      rain.push_back(WeatherModule.getRainAmount());
      radiation.push_back(WeatherModule.getDayRadiation());
           
      }
    /*
    analysis.setfilename(string("hi.txt"));
    analysis.outputRow(his);
    analysis.setfilename(string("lows.txt"));
    analysis.outputRow(lows);
    analysis.setfilename(string("rain.txt"));
    analysis.outputRow(rain);*/
    }
  else
    cout << "Error loading file(s):\n    Rain stats=" << rainStatsTrue << "\n    Temp(hi) stats=" << tempHiStatsTrue
         << "\n    Temp(low) stats=" << tempLowStatsTrue << endl;
  cin.ignore(1);

  return 0;
  }