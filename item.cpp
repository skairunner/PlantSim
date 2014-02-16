#include "item.h"
#include "utility.h"

using namespace HerbSim;
using namespace std;

Item::Item()
: type("Item"), number(1)
{

}

string Item::getName()
{
    return prop.name;
}

int Item::amount()
{
    return number;
}

string Item::amount_s()
{
    return "";
}


Seed::Seed(ALMANAC::PlantProperties& sp)
{
    prop = sp;
}

string Seed::getName()
{
    return prop.name;
}

int c = 0; // Temporary fix to make every seed ID unique.
SeedProperties::SeedProperties(ALMANAC::PlantProperties& properties)
{
    this->itemtype = "seed";
    pp = properties;
    this->name = pp.name + " seed";
    this->ID = this->name + rightAlignNumber(c++);
}

