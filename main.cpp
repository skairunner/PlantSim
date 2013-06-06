//#include "Engine.h"
#include <stdlib.h>
#include "parser_rain.h"
#include <string>
#include "Weather.h"
#include "plant.h"
#include <iostream>

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
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

  Weather WeatherModule(result, false);

  WeatherModule.loadRain(rainMeans, STDEVinfo, skewInfo);
  WeatherModule.loadSun(sunMeans);
  WeatherModule.loadTemps(TempHiInfo, TempLoInfo, STDEVinfo);
  WeatherModule.loadHumidity(HumidityInfo);
  
  WeatherModule.changeDate(JANUARY, 28);

  BasePlant plant;
  for (int counter = 0; counter < 250; counter++)
    {
    WeatherModule.step();
    plant.calculate(WeatherModule.getMaxTemp(), WeatherModule.getMinTemp(), WeatherModule.getDayRadiation(), 440, WeatherModule.getAverageHumidity(), 0.25f, 0);
    cout << WeatherModule.getMonth() << ": " << plant.getHU() << " GDDs," << plant.calcHeight()/1000 << "m, biomass " << plant.getBiomass() * 1000 << " g, LAI " << plant.getLAI() << "\n";
    }
  
  cin.ignore(1);
  return 0;
  }