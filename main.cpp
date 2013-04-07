#include <iostream>
#include "parser_rain.h"
#include "Weather.h"
#include <string>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main()
  {
  Parse::Parser_RainyDays rainyDaysPerMonth(string("Content/unknownRainyDays.rain"));
  Parse::Parser rainStats(string("Content/30yearaverage.rain"));
  bool rainydayspermonthTrue = rainyDaysPerMonth.load();
  bool rainStatsTrue = rainStats.load();
  srand(time(0)); 

  if (rainydayspermonthTrue && rainStatsTrue)
    {
    rainyDaysPerMonth.parse();
    vector<vector<double>> stats = rainStats.parse();
    
    ALMANAC::Weather weatherModule(rainyDaysPerMonth.getResult(), stats, stats, true);
    long double totalRain = 0;

    for (int counter = 0; counter < 30; counter++)
      {
      weatherModule.step(1);
      cout << weatherModule.getMonth() << " : ";
      if (weatherModule.rain()) // if it rained today...
        {
        cout << "It has rained " << weatherModule.getRainAmount() << "mm today.\n";
        totalRain += weatherModule.getRainAmount();
        }
      else
        cout << " It has not rained today.\n";
      }

    cout << "Total rain over time period is " << totalRain << "mm.\n";
    }
  else
    cout << "Rainy days per month loaded: " << rainydayspermonthTrue << "\nRain stats loaded: " << rainStatsTrue << endl;

  cin.ignore(1);

  return 0;
  }