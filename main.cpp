#include "Engine.h"
#include <stdlib.h>
#include <string>
#include "Weather.h"
#include "plant.h"
#include "soilGrid.h"
#include <iostream>
#include <fstream>

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
  {
  Weather WeatherModule(true);
  WeatherModule.changeDate(JANUARY, 28);

  SoilGrid sg(1, 1); 

  BasePlant plant(&sg.ref(0, 0));
  BasePlant plant2(&sg.ref(0, 0));
  plant2.maxLAI = 6;

  fstream file;
  file.open("output1.txt", fstream::out | fstream::trunc);
  file << "Date\tHeat units\tHeight(mm)\tRoot depth(mm)\tBiomass(g)\tLAI\tREG\tSurface water\tWater per layer\n";

  //sg.ref(0, 0).requestWater(1000, 0);
  //sg.addWaterSquare(0, 0, 1, 1, 50);

  sg.step(25);

  for (int counter = 0; counter < 250; counter++)
    {
    WeatherModule.step();
    sg.step(WeatherModule.getRainAmount());

    
   // plant.calculate(WeatherModule.getDataBundle(), 0.25);

    //cout << WeatherModule.getMonth() << ": " << plant.getHU() << " GDDs," << plant.calcHeight()/100 << "cm, biomass " << plant.getBiomass() * 1000 << " g, LAI " << plant.getLAI() << "\n";
    file << WeatherModule.getMonth() << "\t" << plant.getHU() << "\t" << plant.calcHeight() << "\t" << plant.calcRootDepth() << "\t" << plant.getBiomass() * 1000 << "\t" << plant.getLAI() << "\t" << plant.getREG();
    
    file << "\t" << sg.get(0, 0).surfaceWater;
    for each (double val in sg.get(0, 0).inspectWater())
      {
      file << "\t" << val;
      }

    file << "\n";

   // cout << WeatherModule.getMonth() << "\t";
   // cout << sg.ref(0,0);
    /*
    file << WeatherModule.getMonth() << "\t" << WeatherModule.getRainAmount() << "\t";
    file << sg.ref(0, 0);*/

    }

  cout << "Done.";

  cin.ignore(1);
  return 0;

  DisplayEngine.Init(800,640);
	DisplayEngine.Start();
	DisplayEngine.End();
  
  cin.ignore(1);
  return 0;
  }