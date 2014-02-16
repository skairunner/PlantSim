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

#include "curves.h"
#include "item.h"

using namespace ALMANAC;
using namespace std;


int main(int argc, char **argv)
{
    /*fstream f; f.open("logs/seedlist", fstream::out | fstream::trunc);
    Tests::singlePlant(360 * 10, "oak");*/

    // vector<string> list = { "fescue grass", "fescue grass", "fescue grass", "fescue grass", "oak" };
    vector<string> list = { "oak", "oak" };
    Tests::multiplePlants(360 * 50, list);

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