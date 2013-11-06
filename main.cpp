//#include "Engine.h"
#include <stdlib.h>
#include "parser_rain.h"
#include <string>
#include "Weather.h"
#include "plant.h"
#include <iostream>
#include <fstream>

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
  {
  Weather WeatherModule(true);
  WeatherModule.changeDate(JANUARY, 28);

  BasePlant plant;
  fstream file;
  file.open("output.txt", fstream::out | fstream::trunc);
  file << "Date\tHeat units\tHeight(m)\tBiomass(g)\tLAI\n";
  for (int counter = 0; counter < 250; counter++)
    {
    WeatherModule.step();
    plant.calculate(WeatherModule.getMaxTemp(), WeatherModule.getMinTemp(), WeatherModule.getDayRadiation(), 440, WeatherModule.getAverageHumidity(), 0.25f, 0);
    cout << WeatherModule.getMonth() << ": " << plant.getHU() << " GDDs," << plant.calcHeight()/1000 << "m, biomass " << plant.getBiomass() * 1000 << " g, LAI " << plant.getLAI() << "\n";
    file << WeatherModule.getMonth() << "\t" << plant.getHU() << "\t" << plant.calcHeight()/1000 << "\t" << plant.getBiomass() * 1000 << "\t" << plant.getLAI() << "\n";
    }
  
  cin.ignore(1);
  return 0;
  }