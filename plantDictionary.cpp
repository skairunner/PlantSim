#include "plantDictionary.h"
#include <fstream>
#include <iostream>


using namespace ALMANAC;
using namespace std;

PlantDictionary PD;

PlantDictionary::PlantDictionary()
{

}

void PlantDictionary::init()
{


    Json::Reader reader;
    Json::Value root;
    bool success = reader.parse(slurp("Content/plantproperties.json"), root);
    if (!success)
    {
        cout << "Did not succeed: " << reader.getFormattedErrorMessages() << "\n";
        return;
    }

    int counter = 0;
    while (!root[counter].isNull())
    {
        Json::Value plant = root[counter];
        PlantProperties pp;

        pp.name = plant["name"].asString();
        pp.isAnnual = plant["annual"].asBool();
        pp.isTree = false;
        pp.maxLAI = plant["maxLAI"].asDouble();
        pp.growthStages[6] = plant["stage 6"].asDouble();
        pp.growthStages[7] = plant["stage 7"].asDouble();
        pp.growthStages[9] = plant["stage 9"].asDouble();
        pp.growthStages[10] = plant["stage 10"].asDouble();
        pp.baseTemp = plant["base temp"].asDouble();
        pp.waterTolerence = plant["water tolerence"].asDouble();
        pp.maxHeight = plant["max height"].asDouble();
        pp.maxRootDepth = plant["max root depth"].asDouble();
        pp.startingNitrogenConcentration = plant["starting nitrogen"].asDouble();
        pp.finalNitrogenConcentration = plant["final nitrogen"].asDouble();
        pp.dayNeutral = plant["day neutral"].asBool();

        if (!pp.dayNeutral)
        {
            double min = plant["flowering min temp"].asDouble();
            double opt = plant["flowering optimal temp"].asDouble();
            pp.minFloweringTemp = min;
            pp.optimalFloweringTemp = opt;
            pp.flowerTempCurve = Parabola(min, opt, 1);
            pp.floralInductionUnitsRequired = plant["floral induction units"].asDouble();
            pp.longDayPlant = plant["long day"].asBool();
            pp.minimumInduction = plant["minimum induction"].asDouble();
            pp.criticalNightLength = plant["critical night length"].asDouble();
        }
        else
        {
            double min = 0;
            double opt = 25;
            pp.minFloweringTemp = 10;
            pp.optimalFloweringTemp = 25;
            pp.flowerTempCurve = Parabola(min, opt, 1);
            pp.floralInductionUnitsRequired = 0;
            pp.longDayPlant = false;
            pp.minimumInduction = 1;
            pp.criticalNightLength = 12;
        }

        bool useDefaultDormancyValues = false;
        useDefaultDormancyValues = plant["default dormancy"].asBool();
        if (useDefaultDormancyValues)
        {
            pp.dormantHeightDecrease = 0.995;
            pp.dormantHeightDecrease = 0.998;
            pp.dormantBiomassDecrease = BiomassHolder(0.995, 0.998, 0.995, 0.995);
        }
        else
        {
            pp.dormantHeightDecrease = plant["dormant height decrease"].asDouble();
            pp.dormantRootDecrease = plant["dormant root decrease"].asDouble();
            double stem = plant["dormant biomass"]["stem"].asDouble();
            double root = plant["dormant biomass"]["root"].asDouble();
            double storage = plant["dormant biomass"]["storage"].asDouble();
            double fruit = plant["dormant biomass"]["fruit"].asDouble();
            pp.dormantBiomassDecrease = BiomassHolder(stem, root, storage, fruit);

        }

        pp.minimumTemperature = plant["min growth temp"].asDouble();
        pp.optimalTemperature = plant["optimal growth temp"].asDouble();

        pp.tempCurve = Parabola(pp.minimumTemperature, pp.optimalTemperature, 1);

        double stem = plant["base"]["stem"].asDouble();
        double root = plant["base"]["root"].asDouble();
        double storage = plant["base"]["storage"].asDouble();
        double fruit = plant["base"]["fruit"].asDouble();
        pp.baseRatios = BiomassHolder(stem, root, storage, fruit);
        stem = plant["flowering"]["stem"].asDouble();
        root = plant["flowering"]["root"].asDouble();
        storage = plant["flowering"]["storage"].asDouble();
        fruit = plant["flowering"]["fruit"].asDouble();
        pp.fruitingRatios = BiomassHolder(stem, root, storage, fruit);
        stem = plant["final"]["stem"].asDouble();
        root = plant["final"]["root"].asDouble();
        storage = plant["final"]["storage"].asDouble();
        fruit = plant["final"]["fruit"].asDouble();
        pp.finalRatios = BiomassHolder(stem, root, storage, fruit);

        pp.minGerminationTemp = plant["germination min temp"].asDouble();
        pp.optimalGerminationTemp = plant["germination optimal temp"].asDouble();
        pp.averageFruitWeight = plant["average fruit weight"].asDouble();
        pp.seedRatio = plant["seed ratio"].asDouble();
        pp.dormancy = plant["dormancy"].asInt();

        pp.HeatUnitFactorNums = SCurve(1, 17, 0.18);
        pp.CO2CurveFactors = SCurve(0.1, 0.04, 49);

        pp.biomassToVPD = 7;

        list[pp.name] = pp;
        counter++;
    }

    cout << "input done\n";
}

string PlantDictionary::slurp(const string& filename)
{
    fstream file(filename.c_str());
    string output; output.clear();
    string buffer; buffer.clear();
    while (!file.eof())
    {
        std::getline(file, buffer);
        output.append(buffer + string("\n"));
    }
    return output;
}

PlantProperties PlantDictionary::getPlant(const string& plantname)
{
    auto it = list.find(plantname);
    if (it != list.end())
        return it->second;
    return PlantProperties();
}