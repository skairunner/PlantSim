#include "plantDictionary.h"
#include <fstream>
#include <iostream>


using namespace ALMANAC;
using namespace std;

PlantDictionary PD;

PlantDictionary::PlantDictionary()
{
    PD.init();
}

void PlantDictionary::init()
{
    loadProperties();
    loadVisualProperties();
    cout << "input done\n";
}


void PlantDictionary::loadVisualProperties()
{
    Json::Reader reader;
    Json::Value root;
    bool success = reader.parse(slurp("Content/visualproperties.json"), root);
    if (!success)
    {
        cout << "Did not succeed: " << reader.getFormattedErrorMessages() << "\n";
        return;
    }

    int counter = 0;
    while (!root[counter].isNull())
    {
        Json::Value plant = root[counter];
        PlantVisualProperties vp;

        vp.ID = plant["ID"].asString();
        vp.name = plant["name"].asString();
        vp.name_plural = plant["plural"].asString();
        vp.seedname = plant["seed name"].asString();
        vp.seedname_plural = plant["seed plural"].asString();
        vp.isCover = plant["isCover"].asBool();
        vp.whiteBackground = plant["white background"].asBool();
        vp.icon_sprout = plant["sprout icon"].asInt();
        vp.icon_vegetative = plant["vegetative icon"].asInt();
        vp.icon_mature = plant["mature icon"].asInt();
        int r, g, b;
        r = plant["rgb1"]["r"].asInt();
        g = plant["rgb1"]["g"].asInt();
        b = plant["rgb1"]["b"].asInt();
        vp.color1 = RGB(r, g, b);
        r = plant["rgb2"]["r"].asInt();
        g = plant["rgb2"]["g"].asInt();
        b = plant["rgb2"]["b"].asInt();
        vp.color2 = RGB(r, g, b);
        

        visuallist[vp.ID] = vp;
        counter++;
    }
}

// Assigns a range of data from element to var.
void assignPairs(pair<int, int>& var, Json::Value element)
{
    if (element.size() == 1)
    {
        var.first = var.second = element[0].asInt();
    }
    else if (element.size() > 1)
    {
        var.first = element[0].asInt();
        var.second = element[1].asInt();
    }
    else // if element size is 0
    {
        cout << "Missing data\n";
    }
}
void assignPairs(pair<double, double>& var, Json::Value element)
{
    if (element.size() == 1)
    {
        var.first = var.second = element[0].asDouble();
    }
    else if (element.size() > 1)
    {
        var.first = element[0].asDouble();
        var.second = element[1].asDouble();
    }
    else // if element size is 0
    {
        cout << "Missing data\n";
    }
}

void PlantDictionary::loadProperties()
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
        MasterPlantProperties pp;

        pp.name = plant["name"].asString();
        pp.isAnnual = plant["annual"].asBool();
        if (!pp.isAnnual)
        {
            assignPairs(pp.leafFallPeriod_r,  plant["leaf fall period"]);
            assignPairs(pp.maxAge_r, plant["max age"]);
        }
        pp.isTree = plant["is tree"].asBool();
        if (pp.isTree)
        {
            assignPairs(pp.yearsUntilMaturity_r, plant["maturity"]);
            assignPairs(pp.vegetativeMaturity_r, plant["vegetative maturity"]);
            assignPairs(pp.maxYearlyGrowth_r, plant["max yearly growth"]);
        }

        assignPairs(pp.maxLAI_r, plant["maxLAI"]);
        if (plant["uses custom LAI graph"].asBool())
        {
            double leftroot = plant["LAI graph"]["left root"].asDouble();
            double vertexX = plant["LAI graph"]["vertex X"].asDouble();
            double vertexY = plant["LAI graph"]["vertex Y"].asDouble();
            pp.LAIGraph = LeafDistribution(Parabola(leftroot, vertexX, vertexY));
        }
        else
        {
            pp.LAIGraph = LeafDistribution(true);
        }

        pp.growthStages[6] = plant["stage 6"].asDouble();
        pp.growthStages[7] = plant["stage 7"].asDouble();
        pp.growthStages[9] = plant["stage 9"].asDouble();
        pp.growthStages[10] = plant["stage 10"].asDouble();
        pp.baseTemp = plant["base temp"].asDouble();
        assignPairs(pp.waterTolerence_r, plant["water tolerence"]);
        assignPairs(pp.maxHeight_r, plant["max height"]);
        assignPairs(pp.maxRootDepth_r, plant["max root depth"]);
        pp.startingNitrogenConcentration = plant["starting nitrogen"].asDouble();
        pp.finalNitrogenConcentration = plant["final nitrogen"].asDouble();
        pp.dayNeutral = plant["day neutral"].asBool();

        if (!pp.dayNeutral)
        {
            double min = plant["photoinduction"]["min temp"].asDouble();
            double opt = plant["photoinduction"]["optimal temp"].asDouble();
            pp.minFloweringTemp = min;
            pp.optimalFloweringTemp = opt;
            pp.flowerTempCurve = Parabola(min, opt, 1);
            pp.floralInductionUnitsRequired = plant["photoinduction"]["floral induction units"].asDouble();
            pp.longDayPlant = plant["photoinduction"]["long day"].asBool();
            pp.minimumInduction = plant["photoinduction"]["minimum induction"].asDouble();
            pp.criticalNightLength = plant["photoinduction"]["critical night length"].asDouble();
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

        pp.needsVernalization = plant["needs vernalization"].asBool();
        if (pp.needsVernalization)
        {
            pp.isObligateVernalization = plant["vernalization"]["obligate"].asBool();
            pp.minVernalizationTemp = plant["vernalization"]["min temp"].asDouble();
            pp.optimalVernalizationTemp = plant["vernalization"]["optimal temp"].asDouble();
            pp.vernalizationThermalUnits = plant["vernalization"]["thermal units"].asDouble();
            pp.vernalizationCurve = Parabola(pp.minVernalizationTemp, pp.optimalVernalizationTemp, 1);
        }
        else
        {
            pp.isObligateVernalization = false;
            pp.minVernalizationTemp = 0;
            pp.optimalVernalizationTemp = 2.5;
            pp.vernalizationThermalUnits = 45;
            pp.vernalizationCurve = Parabola(pp.minVernalizationTemp, pp.optimalVernalizationTemp, 1);
        }

        bool useDefaultDormancyValues = false;
        useDefaultDormancyValues = plant["default dormancy"].asBool();
        if (useDefaultDormancyValues)
        {
            pp.dormantHeightDecrease = 0.995;
            pp.dormantRootDecrease = 0.998;
            pp.dormantBiomassDecrease = BiomassHolder(0.995, 0.998, 0.995, 0.995);
        }
        else
        {
            pp.dormantHeightDecrease = plant["dormancy"]["height decrease"].asDouble();
            pp.dormantRootDecrease = plant["dormancy"]["root decrease"].asDouble();
            double stem = plant["dormancy"]["biomass"]["stem"].asDouble();
            double root = plant["dormancy"]["biomass"]["root"].asDouble();
            double storage = plant["dormancy"]["biomass"]["storage"].asDouble();
            double fruit = plant["dormancy"]["biomass"]["fruit"].asDouble();
            pp.dormantBiomassDecrease = BiomassHolder(stem, root, storage, fruit);

        }

        pp.minimumTemperature = plant["growth"]["min temp"].asDouble();
        pp.optimalTemperature = plant["growth"]["optimal temp"].asDouble();

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

        pp.minGerminationTemp = plant["germination"]["min temp"].asDouble();
        pp.optimalGerminationTemp = plant["germination"]["optimal temp"].asDouble();
        assignPairs(pp.averageFruitWeight_r, plant["average fruit weight"]);
        assignPairs(pp.seedRatio_r, plant["seed ratio"]);
        pp.dormancy = plant["seed dormancy"].asInt();
        pp.seedViability = plant["seed viability"].asDouble();

        pp.HeatUnitFactorNums = SCurve(1, 17, 0.18);
        pp.CO2CurveFactors = SCurve(0.1, 0.04, 49);

        pp.biomassToVPD = 7;

        propertieslist[pp.name] = pp;
        counter++;
    }
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
    auto it = propertieslist.find(plantname);
    if (it != propertieslist.end())
        return it->second.convert(MendelModule);
    return PlantProperties();
}

PlantVisualProperties PlantDictionary::getVisual(const string& plantname)
{
    auto it = visuallist.find(plantname);
    if (it != visuallist.end())
        return it->second;
    return PlantVisualProperties();
}