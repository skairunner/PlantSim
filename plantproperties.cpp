#include "plantproperties.h"

using namespace ALMANAC;

BiomassHolder::BiomassHolder()
{
    stem = roots = storageOrgan = flowerAndfruits = 0;
}

BiomassHolder::BiomassHolder(double Stem, double Roots, double Storage, double Fruits)
: stem(Stem), roots(Roots), storageOrgan(Storage), flowerAndfruits(Fruits)
{

}

double BiomassHolder::getBiomass() const
{
    return flowerAndfruits + roots + stem + storageOrgan;
}

BiomassHolder::operator double() const
{
    return getBiomass();
}

//////////////
//////////////
//////////////