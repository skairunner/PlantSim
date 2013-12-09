#include "seed.h"
#include "plant.h"
#include <algorithm>

using namespace ALMANAC;

Seed::Seed()
: germinated(false)
{

}

Seed::Seed(const PlantProperties& PP, const Month& Date, const int& dormancy, const double& seed, const double fruit)
: germinated(false)
{
    pp = PP;
    seedBiomass = seed;
    fruitBiomass = fruit;
    dormantDays = dormancy;
    date = Date;

    germinationCurve = Parabola(pp.minGerminationTemp, pp.optimalGerminationTemp, 1.0);
}

bool Seed::attemptGerminate(const WeatherData& wd)
{
    if (germinated)
        return true;

    if (wd.date - date <= dormantDays)
        return false;

    germinationCounter += max(0.0, germinationCurve.getValue((wd.minTemp + wd.maxTemp) / 2.0));
    if (germinationCounter > pp.germinationThermalUnits)
        germinated = true;

    return germinated;
}