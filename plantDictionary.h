#pragma once
#include "json/json.h"
#include "plantproperties.h"
#include <map>
#include <string>

using std::string;

namespace ALMANAC
{
    class PlantDictionary
    {
    public:
        PlantDictionary();
        void init();
        string slurp(const string& filename);
        PlantProperties getPlant(const string& plantname);

    private:
        std::map<string, PlantProperties> list;
    };
}

extern ALMANAC::PlantDictionary PD;