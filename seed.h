#pragma once
#include "plantproperties.h"
#include "Weather.h"
#include "Months.h"
#include "curves.h"

namespace ALMANAC
{
    class BasePlant;

    class Seed
    {
    public:
        Seed();
        Seed(const PlantProperties& PP, const Month& Date, const int& dormancy, const double& seed, const double fruit = 0);
        PlantProperties pp;

        bool attemptGerminate(const WeatherData& wd);
        
        bool germinated;
        int dormantDays;
        
        double seedBiomass;
        double fruitBiomass; // for fleshy fruits like apples.
        Month date;

    protected:
        
        Parabola germinationCurve;
        double germinationCounter;

    };

}