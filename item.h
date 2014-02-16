#pragma once
#include <string>
#include "plantproperties.h"
#include <vector>

namespace HerbSim
{
    struct ItemProperties // inate, unchanging properties of items. Examples include ID, name, genes...
    {
        std::string ID;
        std::string itemtype;
        std::string name;
    };

    struct SeedProperties : public ItemProperties
    {
        SeedProperties(){}
        SeedProperties(ALMANAC::PlantProperties& properties);
        ALMANAC::PlantProperties pp;
    };

    class Item
    {
    public:
        Item();
        std::string type;

        virtual std::string getName();
        virtual int amount();
        virtual std::string amount_s(); // to use when displaying units alongside amount.
        ItemProperties prop;

    protected:
        int number;        
    };

    class Seed
    {
    public:
        Seed(){}
        Seed(ALMANAC::PlantProperties& sp);
        std::string tag;
        std::string getName();
        SeedProperties prop;
    };
}