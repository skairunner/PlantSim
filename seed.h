#pragma once
#include "plantproperties.h"

namespace ALMANAC
{
    class BasePlant;

    class Seed
    {
    public:
        Seed();
        Seed(const PlantProperties& PP, const int& dormancy, const double& seed, const double fruit = 0);
        PlantProperties pp;

        int dormantDays;
        double seedBiomass;
        double fruitBiomass; // for fleshy fruits like apples.
               
    };

}