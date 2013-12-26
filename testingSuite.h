#include <stdlib.h>
#include <string>
#include "Weather.h"
#include "plant.h"
#include "soilGrid.h"
#include <iostream>
#include <fstream>

namespace ALMANAC
{
    using namespace std;
    class Tests
    {
    public:
        static void perPlantingDates(); // Runs the simulation for each plant, changing the start sim day by one day for the whole year.
        static void singlePlant(const int daysToRun = 250, const std::string& plantname = "Pea",  Month startDate = Month(APRIL, 12));
    };
    
}