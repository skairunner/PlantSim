#include "seed.h"
#include "plant.h"

using namespace ALMANAC;

Seed::Seed()
{

}

Seed::Seed(const PlantProperties& PP, const int& dormancy, const double& seed, const double fruit)
{
    pp = PP;
    seedBiomass = seed;
    fruitBiomass = fruit;
    dormantDays = dormancy;
}