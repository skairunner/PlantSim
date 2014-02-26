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
        void loadProperties();
        void loadVisualProperties();
        string slurp(const string& filename);
        PlantProperties getPlant(const string& plantname);
        PlantVisualProperties getVisual(const string& plantname);

    private:
        std::map<string, MasterPlantProperties> propertieslist;
        std::map<string, PlantVisualProperties> visuallist;
    };
}

extern ALMANAC::PlantDictionary PD;