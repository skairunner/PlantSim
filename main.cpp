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
  WeatherModule.changeDate(APRIL, 5);

  //


  SoilGrid sg(1, 1); 


  fstream file;
  file.open("output1.txt", fstream::out | fstream::trunc);
  file << "Date\tHeat units\tHeight(mm)\tRoot depth(mm)\tBiomass(g)\tLAI\tREG\tPrecipitation\tSurface water\tWater per layer\n";

  fstream Nfile;
  Nfile.open("nitrogen.txt", fstream::out | fstream::trunc);
  Nfile << "Date\tBiomass\tPlant nitrogen\tPrecipitation\tNitrogen\n";

  fstream PlantPartFile;
  PlantPartFile.open("plantparts.txt", fstream::out | fstream::trunc);
  PlantPartFile << "Date\tTemp\tRoot\tStem\tStorage\tFruit\tTotal\tHUF\tFloral induction\n";

  //sg.ref(0, 0).requestWater(1000, 0);
  //sg.addWaterSquare(0, 0, 1, 1, 50);
  sg.ref(0, 0).plants.push_back(BasePlant(&sg.ref(0, 0)));

  sg.step(25);

  for (int counter = 0; counter < 250; counter++)
    {
    WeatherModule.step();
    sg.step(WeatherModule.getRainAmount());
    sg.stepPlants(WeatherModule.getDataBundle());


    BasePlant plant = sg.ref(0, 0).plants.front();

    //// output plant stats    
    file << WeatherModule.getMonth() << "\t" << plant.getHU() << "\t" << plant.calcHeight() << "\t" << plant.calcRootDepth() << "\t" << plant.getBiomass() * 1000 << "\t" << plant.getLAI() << "\t" << plant.getREG();
   
    file << "\t" << WeatherModule.getRainAmount() << "\t" << sg.get(0, 0).surfaceWater;
    for (double val : sg.get(0, 0).inspectWater())
      {
      file << "\t" << val;
      }  

    file << "\n";

    //// output nitrogen stats
    Nfile << WeatherModule.getMonth() << "\t" << plant.getBiomass() << "\t" << plant.getNitrogen() << "\t" << WeatherModule.getRainAmount();
    for (double val : sg.get(0, 0).inspectNitrates())
    {
        Nfile << "\t" << val;
    }

    Nfile << "\n";


    //// output biomass per plant part
    BiomassHolder biomass = plant.getBiomassStruct();
    PlantPartFile << WeatherModule.getMonth() << "\t" << (WeatherModule.getMaxTemp() + WeatherModule.getMinTemp())/2.0 << "\t" << biomass.roots 
        << "\t" << biomass.stem << "\t" << biomass.storageOrgan << "\t" << biomass.flowerAndfruits << "\t" << biomass << "\t" << plant.findHUI()
        << "\t" << plant.getInduction() << "\t" << plant.getHU() << "\n";
    }

  sg.ref(0, 0).plants.front().createSeeds(WeatherModule.getMonth());
 
  auto seeds = sg.ref(0, 0).plants.front().seedlist;

  int seedcounter = 0;
  for (auto seed : seeds)
  {
      seedcounter++;
      cout << "Seed " << seedcounter << " biomass: " << seed.seedBiomass * 1000 << "g\n";
  }



  /////

  cout << sg.ref(0, 0).plants.front().getBiomass();
  cout << "\nDone.";

  cin.ignore(1);
  return 0;

  DisplayEngine.Init(800,640);
  DisplayEngine.Start();
  DisplayEngine.End();
  
  cin.ignore(1);
  return 0;
  }