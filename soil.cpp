#include "soil.h"
#include <cmath>
#include "enums.h"
#include <cassert>
#include <iostream>
#include <algorithm>

using namespace ALMANAC;
using namespace std;

const double BL = 20; // nitrogen leaching parameter

const double SoilLayer::soilWeight = 1200;


SoilLayer::SoilLayer(const double& sandi, const double& clayi, const double& silti, const double& organicMatteri, unsigned int thickness)
:sand(sandi), clay(clayi), silt(silti), organicMatter(organicMatteri), depth(thickness), isTopsoil(false), isAquifer(false)
, water(0), nitrates(5.604), organicMatterWeight(50.4), plantmatter(0)
{
    properties = SOIL_MODELS::SoilModule::fetch(sand, silt, clay, organicMatter);

}

double SoilLayer::getDepth()
{
    return depth;
}

void SoilLayer::percolateAndLateral(const double& slope)
{

    percolateDown = 0;
    percolateUp = 0;
    lateral = 0;
    if (water > fieldCapacity() + 0.000001f)
    {
        double lateralFlowTime = travelTime() / (slope / 1000.0f);
        double initialperc = (water - fieldCapacity())*(1 - exp(-travelTime() * 24 - lateralFlowTime * 24));// "O + QH"
        percolateDown = initialperc / (1 + 24 * travelTime());
        lateral = initialperc / (1 + 24 * lateralFlowTime);
    }

    previousWater = percolationWater();

    movedNitrates = findMovedNitrates(percolateDown + percolateUp + lateral);
    nitrates -= movedNitrates;
}

double SoilLayer::totalWaterMoved()
{
    return percolateDown + percolateUp + lateral;
}

double SoilLayer::totalWater()
{
    return water;
}

double SoilLayer::wiltingPoint()
{
    return properties.wiltingPoint * depth;
}

double SoilLayer::fieldCapacity()
{
    return properties.fieldCapacity * depth;
}

double SoilLayer::saturatedMoisture()
{
    return properties.saturatedMoisture * depth;
}

double SoilLayer::SatHydConductivity()
{
    return properties.SatHydConductivity;
}

double SoilLayer::travelTime()
{
    return properties.travelTime;
}

void SoilLayer::addWater(const double& addwater)
{
    water += addwater;
}

double SoilLayer::availableWater()
{
    return (water - wiltingPoint()) > 0 ? (water - wiltingPoint()) : 0;
}

double SoilLayer::percolationWater()
{
    return (water - fieldCapacity()) > 0 ? (water - fieldCapacity()) : 0;
}

double SoilLayer::withdrawWater(const double& amount, const int numPlants)
{
    return withdrawWater(amount, depth, numPlants);
}

double SoilLayer::withdrawWater(const double& amount, const double& rootdepth, const int numPlants)
{
    double supply = 0;
    double depthFraction = rootdepth / depth;
    if (depthFraction > 1) depthFraction = 1;
    double actualAvailableWater = availableWater() * depthFraction;

    if (numPlants != 0 && amount > actualAvailableWater / numPlants)
    {
        supply = actualAvailableWater / numPlants;
        water -= supply;
    }
    else if (amount > actualAvailableWater)
    {
        supply = actualAvailableWater;
        water -= actualAvailableWater;
    }
    else
    {
        supply = amount;
        water -= amount;
    }

    return supply;
}

double SoilLayer::getSoilWeight()
{
    return soilWeight / 1000 * depth;
}

double SoilLayer::withdrawNitrogen(const double& amount, const double& waterUptake)
{
    return withdrawNitrogen(amount, waterUptake, depth);
}

double SoilLayer::withdrawNitrogen(const double& amount, const double& waterUptake, const double& rootDepth)
{
    double supply = 0;

    double maxNitrogen = waterUptake / water * nitrates;

    if (amount > maxNitrogen)
    {
        nitrates -= maxNitrogen;
        supply = maxNitrogen;
    }
    else
    {
        supply = amount;
        nitrates -= amount;
    }


    return supply;
}

void SoilLayer::denitrification(const double temp)
{
    double SWF = water / fieldCapacity(); // soil water / field capacity
    double denitr = 0;
    double tempFactor = max(0.1, temp / (temp + exp(9.93 - 0.312 * temp)));
    if (SWF > 0.95)
        denitr = nitrates * (1.0 - exp(-1.4 * tempFactor * organicMatterWeight / getSoilWeight()));

    nitrates -= denitr;
}

double SoilLayer::findMovedNitrates(const double& waterVolume)
{
    return nitrates * (1 - exp(-waterVolume / (saturatedMoisture() * BL)));
}

void SoilLayer::adjustWater()
{
    if (isTopsoil) percolateUp = 0;


    double waterout = lateral + percolateDown + percolateUp;
    double mult = 0;

    if (waterout > previousWater) // if water out is larger than available water.
    {
        mult = previousWater / waterout;
        lateral *= mult;
        percolateUp *= mult;
        percolateDown *= mult;
    }
}


SoilCell::SoilCell()
: surfaceWater(0), snow(0)
{
    for (int counter = 0; counter < 8; counter++)
        flowInputs[counter] = 0;
}

void SoilCell::upwardsFlow()
{
    Layers.front().percolateUp = 0;
    for (auto it = Layers.begin() + 1; it < Layers.end(); it++)
    {
        if (it->water > it->fieldCapacity())
            it->percolateUp = (it->percolationWater())*(1 - exp(0.05*((it - 1)->water / (it - 1)->fieldCapacity() - it->water / it->fieldCapacity())));
        if (it->percolateUp < 0)
            it->percolateUp = 0;
    } // for each layer, run the algorithm. Exclude the top layer.
}

vector<double> SoilCell::inspectWater()
{
    vector<double> output;
    for (auto i = Layers.begin(); i < Layers.end(); i++)
    {
        output.push_back(i->availableWater());
    }
    return output;
}

vector<double> SoilCell::inspectNitrates()
{
    vector<double> output;
    for (auto i = Layers.begin(); i < Layers.end(); i++)
    {
        output.push_back(i->nitrates);
    }
    return output;
}

void SoilCell::doSnowmelt(const double temp)
{
    if (temp > 0)
    {
        double meltPercentage = log(temp + 1.0) / log(10.0);
        meltPercentage = min(1.0, meltPercentage);
        double meltedSnow = snow * meltPercentage;
        snow -= meltedSnow;
        surfaceWater += meltedSnow;
    }
    else
    {
        double meltPercentage = log(temp + 1.0) / log(10.0);
        meltPercentage = min(1.0, meltPercentage);
        double meltedSnow = snow * 0.01;
        snow -= meltedSnow;
        surfaceWater += meltedSnow;
    }

    if (snow < 0.00001)
        snow = 0;
}

void SoilCell::solveAndPercolate()
{
    // First add whatever surface water possible to the top layer. Need to implement runoff sooner or later.
    double possibleWater = Layers.front().saturatedMoisture() - Layers.front().water;
    if (surfaceWater < possibleWater)
        possibleWater = surfaceWater;
    surfaceWater -= possibleWater;
    Layers.front().addWater(possibleWater);

    // percolate ALL the layers
    for (auto it = Layers.begin(); it < Layers.end(); it++)
        it->percolateAndLateral(slope);
    upwardsFlow();

    // Move all the intracell water. Do not touch the intercell water.
    for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
        it->adjustWater();

        if (!it->isAquifer) //perc down
        {
            // First, check.    

            double possible = it->percolateDown;
            if ((it + 1)->totalWater() + it->percolateDown >(it + 1)->saturatedMoisture()) // make sure not to add too much water than it can phsyicall hold
            {
                possible = (it + 1)->saturatedMoisture() - (it + 1)->totalWater();
            }

            (it + 1)->addWater(possible);
            it->addWater(-possible);

            if (it->totalWaterMoved() > 0)
                (it + 1)->nitrates += it->movedNitrates / it->totalWaterMoved() * possible;
        }

        if (!it->isTopsoil) //perc up
        {
            double possible = it->percolateUp;
            if ((it - 1)->totalWater() + it->percolateUp > (it - 1)->saturatedMoisture()) // make sure not to add too much water than it can phsyicall hold
            {
                possible = (it - 1)->saturatedMoisture() - (it - 1)->totalWater();
            }

            (it - 1)->addWater(possible);
            it->addWater(-possible);

            if (it->totalWaterMoved() > 0)
                (it - 1)->nitrates += it->movedNitrates / it->totalWaterMoved() * possible;
        }

        if (it->isAquifer)
        {
            it->addWater(-it->percolateDown); // Simply remove the water that moved down.
            // Recharge the water.
            it->recharge();
        }

        //Finally, lateral is always done.
        it->addWater(-it->lateral);
    }
}

void SoilCell::calculateNitrogen(const double temp)
{
    // denitrification
    /*for (auto layer = Layers.begin(); layer < Layers.end(); layer++)
    {
    layer->denitrification(temp);
    }*/
    // currently removed because the denitrification rate is too severe
}

std::vector<SoilLayer> SoilCell::getLayers()
{
    return Layers;
}

void SoilCell::addWater(const int& layer, const double& amount)
{
    if (layer < Layers.size())
        Layers[layer].addWater(amount);
}

std::ostream& operator<< (std::ostream& o, ALMANAC::SoilCell& sc)
{
    auto copy = sc.getLayers();
    for (auto it = copy.begin(); it < copy.end(); it++)
        o << it->nitrates << "\t";
    o << "\n";
    return o;
}

void SoilCell::calcTotalHeight()
{
    double total = 0;
    for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
        total += it->depth;
    }
    total += baseHeight;
    totalHeight = total;
}

double SoilCell::getTotalHeight()
{
    return totalHeight;
}

soiltuple SoilCell::getTopLayer()
{
    soiltuple output;
    output.sand = Layers.front().sand;
    output.silt = Layers.front().silt;
    output.clay = Layers.front().clay;
    return output;
}

int SoilCell::getTopsoilType()
{
    return topsoilType;
}

void SoilCell::setMooreDirection(const int& moore)
{
    MooreDirection = moore;
}

int SoilCell::getMooreDirection()
{
    return MooreDirection;
}

void SoilCell::transferLateralWater(std::vector<SoilLayer>& OutLayers)
{
    assert(Layers.size() == OutLayers.size());
    auto otherit = OutLayers.begin();
    for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
        otherit->addWater(it->lateral);
        otherit++;
    }
}

double SoilCell::requestWater(double rootDepth, double demand)
{
    double suppliedWater = 0;
    for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
        if (rootDepth - it->getDepth() > 0)
        {
            rootDepth -= it->getDepth();
            suppliedWater += it->withdrawWater(demand / Layers.size());
        }
        else
        {
            suppliedWater += it->withdrawWater(demand / Layers.size(), rootDepth);
        }
    }
    // currently, excess root length does nothing for the plant.

    return suppliedWater;
}

double SoilCell::requestNitrogen(double rootDepth, double demand, double suppliedWater)
{
    double suppliedNitrogen = 0;
    double nitPerLayer = demand / Layers.size();
    double waterPerLayer = suppliedWater / Layers.size();

    if (suppliedWater < 0)
        suppliedWater = 0;

    for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
        if (rootDepth - it->getDepth() > 0)
        {
            rootDepth -= it->getDepth();
            suppliedNitrogen += it->withdrawNitrogen(nitPerLayer, suppliedWater);
        }
        else
        {
            suppliedNitrogen += it->withdrawNitrogen(nitPerLayer, suppliedWater, rootDepth);
        }
    }
    // currently, excess root length does nothing for the plant.

    return suppliedNitrogen;
}

SoilCell* SoilFactory::createTestCell()
{
    SoilCell* output = new SoilCell();
    output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
    output->Layers.back().isTopsoil = true;
    output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.6, 0.2, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.5, 0.35, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.4, 0.4, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.5, .35, 0.1, 0));
    output->Layers.push_back(SoilLayer(0.46, 0.4, 0.1, 0.01));
    output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
    output->Layers.back().isAquifer = true;
    output->totalHeight = 200 * output->Layers.size();
    output->slope = 0.2;

    return output;
}

SoilCell SoilFactory::createCell(const double& baseheight, const double& depth, std::vector<soiltuple>& st)
{
    SoilCell output;
    for (auto it = st.begin(); it < st.end(); it++)
    {
        output.Layers.push_back(SoilLayer(it->sand, it->silt, it->clay, 0.05, depth));
        if (it == st.begin())
            output.Layers.back().isTopsoil = true;
        else if (it == st.end() - 1)
            output.Layers.back().isAquifer = true;

        output.Layers.back().addWater(output.Layers.back().fieldCapacity()); // initialize with water equal to FC.
    }
    output.baseHeight = baseheight;
    output.topsoilType = findTopsoilType(*(st.begin()));
    output.calcTotalHeight();
    return output;
}

int SoilFactory::findTopsoilType(const soiltuple& st)
{
    double clay = st.clay * 100;
    double sand = st.sand * 100;
    double silt = st.silt * 100;

    //first, the simple types
    //clay
    if ((clay >= 40) & (sand <= 55) & (silt <= 40))
        return stCLAY;
    else if (sand >= 90)	//sand
        return  stSAND;
    else if ((silt >= 80) & (clay <= 87)) //silt
        return  stSILT;
    else if ((clay >= 35) & (sand >= 55)) //sandy clay
        return  stSANDYCLAY;
    else if ((clay >= 40) & (silt >= 40)) //silty clay
        return  stSILTYCLAY;
    else if ((clay <= 40) & (clay >= 28) & (sand <= 48) & (sand >= 20)) //clay loam
        return  stCLAYLOAM;
    else if ((clay <= 40) & (clay >= 28) & (sand <= 20)) //silty loam
        return  stSILTYCLAYLOAM;
    else if ((clay <= 35) & (clay >= 20) & (sand >= 55) & (silt <= 28)) //sandy clay loam
        return  stSANDYCLAYLOAM;
    else if (silt >= 50) //silty loam
        return  stSILTLOAM;
    else if ((clay <= 28) & (clay >= 5) & (silt <= 50) & (silt >= 28) & (sand <= 52)) //loam
        return  stLOAM;
    else if ((sand < 90) & (silt <= 8 / 3 * -1 * clay + 40)) //loamy sand
        return  stLOAMYSAND;
    else
        return  stSANDYLOAM; //sandy loam
}

SoilLayer& SoilCell::getFront(const int& offset)
{
    return *(Layers.begin() + offset);
}

void SoilCell::addNitrogenToTop(const double& amount)
{
    if (Layers.size() != 0)
        Layers.front().nitrates += amount;
}

void SoilLayer::recharge()
{
    if (isAquifer)
    {
        if (water < fieldCapacity())
            water += min((fieldCapacity() - water) / fieldCapacity() * 10, fieldCapacity() - water);
    }
}
