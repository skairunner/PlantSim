#include "item.h"
#include "utility.h"

using namespace HerbSim;
using namespace std;

/*Item::Item()
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
}*/


Seed::Seed(ALMANAC::PlantProperties& sp, ALMANAC::PlantVisualProperties& vp)
{
    prop.pp = sp;
    prop.vp = vp;
}

string Seed::getName()
{
    return prop.vp.seedname;
}

int c = 0; // Temporary fix to make every seed ID unique.
SeedProperties::SeedProperties(ALMANAC::PlantProperties& properties)
{
    this->itemtype = "seed";
    pp = properties;
    this->name = pp.name + " seed";
    this->ID = this->name + " " + rightAlignNumber(c++);
}


MultiSeed::MultiSeed(const Seed& s)
{
    seeds.push_back(s);
}

MultiSeed::MultiSeed(ALMANAC::PlantProperties& sp, ALMANAC::PlantVisualProperties& vp)
{
    *this = MultiSeed(Seed(sp, vp));
}

std::string MultiSeed::getName()
{
    if (seeds.size() > 1)
    {
        return to_string(seeds.size()) + seeds.front().prop.vp.seedname_plural;
    }
    else if (seeds.size() == 1)
    {
        return seeds.front().prop.vp.seedname;
    }
    else
        return "#ERROR";
}

int MultiSeed::getNumber()
{
    return seeds.size();
}

ALMANAC::RGB MultiSeed::getColor()
{
    return seeds.front().prop.vp.getColor();
}

Seed& MultiSeed::operator[](const int& index)
{
    if (seeds.size() > index)
        return seeds[index];
    else
        throw std::out_of_range("seeds array out of bounds");
}