#include "Engine.h"
#include <stdlib.h>
#include <string>
#include "Weather.h"
#include "plant.h"
#include "soilGrid.h"
#include <iostream>
#include <fstream>
#include "testingSuite.h"

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
{

    Tests::perPlantingDates();
    return 0;

    Tests::singlePlant();
    return 0;

    /////


    DisplayEngine.Init(800, 640);
    DisplayEngine.Start();
    DisplayEngine.End();

    cin.ignore(1);
    return 0;
}