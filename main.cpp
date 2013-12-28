#include "Engine.h"
#include <stdlib.h>
#include <string>
#include "Weather.h"
#include "plant.h"
#include "soilGrid.h"
#include <iostream>
#include <fstream>
#include "testingSuite.h"

#include "plantDictionary.h"

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
{
    PD.init();
    /*Tests::perPlantingDates();
    return 0;*/


//    Tests::singlePlant(360 * 10, "Pea");
    Tests::singlePlant(360 * 10, "Fescue grass");
    //Tests::singlePlant(360*10, "Cabbage");
    
    cin.ignore(0);
    return 0;

    /////

    DisplayEngine.Init(800, 640);
    DisplayEngine.Start();
    DisplayEngine.End();

    cin.ignore(1);
    return 0;
}