#pragma once
#include "plantproperties.h"

namespace ALMANAC
{
    class BasePlant;

    class Seed
    {
    public:
        Seed();
        Seed(BasePlant* bp);
        PlantProperties pp;

        int dormantDays;
    private:
               
    };

}