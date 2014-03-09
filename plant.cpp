#include "plant.h"
#include "soil.h"
#include "seed.h"

#include <cmath>
#include <algorithm>

#include <fstream>
#include "plantDictionary.h"
using namespace ALMANAC;






BasePlant::BasePlant(SoilCell* soil)
: Biomass(0.05, 0, 0, 0), LAI(0), prevLAI(0), previousHeatUnits(0), heatUnits(0), soilPatch(soil), requiredWater(1), suppliedWater(1), height(0)
, currentWaterlogValue(0), nitrogen(0), floralInductionUnits(0), tempstress(1), rootDepth(0), dead(false), deadBiomass(0), maxBiomass(0), removedNitrogen(0), consecutiveDormantDays(0),
vernalizationUnits(0), age(0), daysLeftForShedding(0), readyForLeafShed(false)
{
    maxBiomass = Biomass;
    *this = BasePlant(PD.getPlant("fescue grass"), PD.getVisual("fescue grass"), soil);
    rng.seed(rand());
}

BasePlant::BasePlant(PlantProperties plantprop, PlantVisualProperties visualprop, SoilCell* soil)
: LAI(0), prevLAI(0), previousHeatUnits(0), heatUnits(0), soilPatch(soil), requiredWater(1), suppliedWater(1), height(0)
, currentWaterlogValue(0), nitrogen(0), floralInductionUnits(0), tempstress(1), rootDepth(0), dead(false), REG(0), deadBiomass(0), removedNitrogen(0), consecutiveDormantDays(0),
vernalizationUnits(0), age(0), daysLeftForShedding(0), readyForLeafShed(false)
{
    prop = plantprop;
    vp = visualprop;
    Biomass = BiomassHolder(prop.averageFruitWeight() * prop.seedRatio() / 10, 0, 0, 0);
    maxBiomass = Biomass;

    nitrogen = findRequiredNitrogen();

    floweringHU = prop.growthStages[6];
    endFloweringHU = prop.growthStages[7];
    finalHU = prop.growthStages[9];
    maxHU = prop.growthStages[10];
    prop.nightLengthCurve = getSCurve(prop.dayNeutral, prop.longDayPlant, prop.minimumInduction, prop.criticalNightLength);

    rng.seed(rand());
}

BasePlant::BasePlant(Seed seed, SoilCell* soil)
: LAI(0), prevLAI(0), previousHeatUnits(0), heatUnits(0), soilPatch(soil), requiredWater(1), suppliedWater(1), height(0)
, currentWaterlogValue(0), nitrogen(0), floralInductionUnits(0), tempstress(1), rootDepth(0), dead(false), REG(0), deadBiomass(0), removedNitrogen(0), consecutiveDormantDays(0),
vernalizationUnits(0), age(0), daysLeftForShedding(0), readyForLeafShed(false)
{
    prop = seed.pp;
    vp = seed.vp;
    Biomass = BiomassHolder(seed.seedBiomass / 10.0, 0, 0, 0);
    maxBiomass = Biomass;

    nitrogen = findRequiredNitrogen();

    floweringHU = prop.growthStages[6];
    endFloweringHU = prop.growthStages[7];
    finalHU = prop.growthStages[9];
    maxHU = prop.growthStages[10];

    rng.seed(rand());
}

double BasePlant::random(double min, double max)
{
    if (min > max)
    {
        double temp = min;
        min = max;
        max = min;
    }

    uniform_real_distribution<> dist(min, max);
    return dist(rng);
}

SCurve BasePlant::getSCurve(const bool dayNeutral, const bool longDayPlant, double minInduction, const double& optimalInductionNightLength)
{
    double v, s, h, y;

    if (minInduction > 1) minInduction = 1;
    if (minInduction < 0) minInduction = 0;
    if (dayNeutral) minInduction = 1;

    // if min induction is 1, v = 0 and y = 1.
    v = 1.0 - minInduction;
    y = minInduction;
    s = 4.0;
    h = optimalInductionNightLength;

    if (longDayPlant)
        s *= -1;

    return SCurve(v, s, h, y);
}

void BasePlant::findREG()
{
    REG = 1;
    REG = min(REG, getWaterStressFactor());
    REG = min(REG, getWaterlogStressFactor());
    REG = min(REG, getNitrogenStressFactor());
    REG = min(REG, max(tempstress, 0.0));
}


void BasePlant::calculate(const WeatherData& data, const double& albedo, const double radiation)
{
    age += 1;
    double heatUnitsAdded = (data.maxTemp + data.minTemp) / 2 - prop.baseTemp;
    heatUnitsAdded = heatUnitsAdded > 0 ? heatUnitsAdded : 0;
    previousHeatUnits = heatUnits;


    if (heatUnitsAdded + heatUnits > maxHU) // If adding HU will go over the limit,
     {
        if (prop.isAnnual)
        {
            dead = true;
        }       
        heatUnitsAdded = 0;
        heatUnits = maxHU;
        createSeeds(data.date);
        readyForLeafShed = true;
       /* if (!prop.isAnnual) // test
            seedlist.clear();*/            
    }


    if (readyForLeafShed && prop.leafFallPeriod() > 0 && prop.tempCurve.getValue(data.maxTemp) < 0.05)
    {
        daysLeftForShedding = prop.leafFallPeriod();
        LAIShedPerDay = LAI / daysLeftForShedding;
        readyForLeafShed = false;
    }

    if (daysLeftForShedding > 0)
    {
        prevLAI = LAI;
        LAI -= LAIShedPerDay;
        daysLeftForShedding--;

        if (daysLeftForShedding == 0)
        {
            LAI = 0;
        }
    }

    if (isDead())
    {
        reduceStandingBiomass(data);
        prevLAI = LAI = 0;
        return;
    }


    //const double tolerence = 0.00005; // plants less than 1 gram are proclaimed to be dead.

    if (isDormant()) // for decreasing standing biomass.
        doDormancy();
    else
        consecutiveDormantDays = 0;
    

    if (heatUnits > finalHU)
    {
        heatUnits += heatUnitsAdded;
    }
    else if (heatUnits < finalHU && !isDead())
    {
        doWater(data);
        doNitrogen();
        doVernalization(data);
        doFloralInduction(data);
        doTempStress(data);
        findREG();


        heatUnits += heatUnitsAdded;
        double deltaHUF = findHUF() - findPreviousHUF();
        prevLAI = LAI;
        LAI += deltaHUF * prop.maxLAI() * (1 - exp(5.0f * (prevLAI - prop.maxLAI()))) * sqrt(REG);
        rootDepth += deltaHUF * prop.maxRootDepth() * sqrt(REG);

        if (prop.isTree)
        {
            height += deltaHUF * prop.maxYearlyGrowth() * sqrt(REG);
            if (height > prop.maxHeight())
                height = prop.maxHeight();

            rootDepth += deltaHUF * prop.maxRootDepth() * sqrt(REG);
            if (rootDepth > prop.maxRootDepth()) rootDepth = prop.maxRootDepth();
        }           
        else
            height += deltaHUF * prop.maxHeight() * sqrt(REG);    

        ///////////////////////
        double photoactiveRadiation;
        if (radiation == -1)
            double photoactiveRadiation = 0.5 * data.radiation * (1 - exp(-0.65 * LAI));
        else
            photoactiveRadiation = radiation;
     
        prop.biomassToVPD = 72;
        double potentialDeltaBiomass = 100 * prop.CO2CurveFactors.getValue(data.CO2); // BE*
        potentialDeltaBiomass = potentialDeltaBiomass - prop.biomassToVPD * (findVPD((data.maxTemp - data.minTemp) / 2.0f, data.humidity) - 1); // BE'
        potentialDeltaBiomass = 0.001f * potentialDeltaBiomass * photoactiveRadiation / 10.0f; //result is in kg / m^2  




        potentialDeltaBiomass *= REG;
        //test
        potentialDeltaBiomass -= min(0.005, Biomass * 0.002);
        partitionBiomass(potentialDeltaBiomass);

        currentWaterlogValue -= 0.005;
        if (currentWaterlogValue < 0)
            currentWaterlogValue = 0;

        if (maxBiomass < Biomass)
            maxBiomass = Biomass;
    }
}

double BasePlant::getInduction()
{
    return floralInductionUnits;
}

double BasePlant::getVernalizedRatio()
{
    if (!prop.needsVernalization)
        return 1;
    if (!prop.isObligateVernalization || vernalizationUnits > prop.vernalizationThermalUnits)
        return vernalizationUnits / prop.vernalizationThermalUnits;
    return 0;
}

double BasePlant::getRequiredWater()
{
    return requiredWater;
}

double BasePlant::getWaterREG()
{
    return currentWaterlogValue / prop.waterTolerence();
}

BiomassHolder BasePlant::getBiomassStruct()
{
    return Biomass;
}

void BasePlant::doWater(const WeatherData& data)
{
    if (soilPatch)
    {
        double potentialEvaporation = 0.0032 * data.radiation / 2.456 /*MJ/kg*/ * ((data.maxTemp + data.minTemp) / 2 + 17.8) * pow(data.maxTemp - data.minTemp, 0.6); // Hargreaves method.
        requiredWater = min(potentialEvaporation, potentialEvaporation * LAI);
        suppliedWater = soilPatch->requestWater(calcRootDepth(), requiredWater);

        // next add waterlog damage
        auto layers = soilPatch->getLayers();
        double depthSum = 0;
        for (SoilLayer& layer : layers)
        {
            depthSum += layer.getDepth();

            if (layer.totalWater() > layer.fieldCapacity() && calcRootDepth() > depthSum)
            {
                double tentativeValue = (layer.totalWater() - layer.fieldCapacity()) / (layer.saturatedMoisture() - layer.fieldCapacity());
                if (tentativeValue > 0.5f)
                    currentWaterlogValue += (tentativeValue - 0.5f) / 10.0;
                // How much extra water over the field capacity there is.
            }
        }
    }
    else
    {
        requiredWater = 0;
        suppliedWater = 0;
    }
}

void BasePlant::doNitrogen()
{
    nitrogen += soilPatch->requestNitrogen(calcRootDepth(), findRequiredNitrogen(), suppliedWater);
}

void BasePlant::doFloralInduction(const WeatherData& data)
{
    if (heatUnits > maxHU * 0.2 /*floweringHU*/ && heatUnits < endFloweringHU)
        floralInductionUnits += max(0.0, prop.flowerTempCurve.getValue((data.maxTemp + data.minTemp) / 2.0) * prop.nightLengthCurve.getValue(data.nightLength));
}

void BasePlant::doVernalization(const WeatherData& data)
{
    vernalizationUnits += max(0.0, prop.vernalizationCurve.getValue((data.maxTemp + data.minTemp) / 2.0));
}

void BasePlant::doTempStress(const WeatherData& data)
{
    tempstress = prop.tempCurve.getValue((data.maxTemp + data.minTemp) / 2.0);
    //tempstress = sin(3.1415 / 2.0 * tempstress);
}

void BasePlant::partitionBiomass(const double dBiomass)
{
    double root, fruit, storage, shoot;

    if (heatUnits < floweringHU)
    {
        double ratio = heatUnits / floweringHU;
        root = prop.baseRatios.roots * (1 - ratio) + prop.fruitingRatios.roots * ratio;
        fruit = prop.baseRatios.flowerAndfruits * (1 - ratio) + prop.fruitingRatios.flowerAndfruits * ratio;
        storage = prop.baseRatios.storageOrgan * (1 - ratio) + prop.fruitingRatios.storageOrgan * ratio;
        shoot = prop.baseRatios.stem * (1 - ratio) + prop.fruitingRatios.stem * ratio;
        /*
        root = 0.4 - 0.2 * heatUnits / floweringHU;
        fruit = 0 + 0.1 * heatUnits / floweringHU;
        storage = 0 + 0.1 * heatUnits / floweringHU;
        shoot = 0.6;*/
    }
    else if (heatUnits < finalHU) // while flowering stage...
    {
        double ratio = (heatUnits - floweringHU) / (finalHU - floweringHU);

        root = prop.fruitingRatios.roots * (1 - ratio) + prop.finalRatios.roots * ratio;
        fruit = prop.fruitingRatios.flowerAndfruits * (1 - ratio) + prop.finalRatios.flowerAndfruits * ratio;
        storage = prop.fruitingRatios.storageOrgan * (1 - ratio) + prop.finalRatios.storageOrgan * ratio;
        shoot = prop.fruitingRatios.stem * (1 - ratio) + prop.finalRatios.stem * ratio;


        /*root = 0.2 - 0.1 * ratio;

        storage = 0.1 + 0.2 * ratio;
        shoot = 0.6 - 0.5 * ratio;

        fruit = 0.1 + 0.4 * ratio;*/

        if (!canFlower()) // if it's not able to flower, route all of the biomass into the other parts of the plant.
        {
            root += fruit / 3;
            storage += fruit / 3;
            shoot += fruit / 3;
            fruit = 0;
        }

        fruit *= getVernalizedRatio(); // Reduce the efficiency of the flowering depending on its vernalization status.
    }
    else
    {
        root = prop.finalRatios.roots;
        fruit = prop.finalRatios.flowerAndfruits;
        storage = prop.finalRatios.storageOrgan;
        shoot = prop.finalRatios.stem;

        if (!canFlower())
        {
            root += fruit / 3;
            storage += fruit / 3;
            shoot += fruit / 3;
            fruit = 0;
        }

        fruit *= getVernalizedRatio();
    }

    Biomass.roots += dBiomass * root;
    Biomass.stem += dBiomass * shoot;
    Biomass.storageOrgan += dBiomass * storage;
    Biomass.flowerAndfruits += dBiomass * fruit;

    if (Biomass.roots < 0)
        Biomass.roots = 0;
    if (Biomass.stem < 0)
        Biomass.stem = 0;
    if (Biomass.storageOrgan < 0)
        Biomass.storageOrgan = 0;
    if (Biomass.flowerAndfruits < 0)
        Biomass.flowerAndfruits = 0;

    /*if (prop.isAnnual)
    {
        Biomass.flowerAndfruits += Biomass.storageOrgan;
        Biomass.storageOrgan = 0;
    }    */
}

void BasePlant::doDormancy()
{
    const double tolerence = 0.00005;
    consecutiveDormantDays++;
    floralInductionUnits = 0;

    if (true)//!prop.isTree) // current dummy function.
    {
        height *= prop.dormantHeightDecrease;
        rootDepth *= prop.dormantRootDecrease;

        if (Biomass < tolerence)
            dead = true;
        else if (Biomass - Biomass.storageOrgan > tolerence) // there are other parts of the plant left
        {
            // consume said parts
            Biomass.flowerAndfruits *= prop.dormantBiomassDecrease.flowerAndfruits;
            Biomass.roots *= prop.dormantBiomassDecrease.roots;
            Biomass.stem *= prop.dormantBiomassDecrease.stem;
        }
        else // otherwise consume the storage organ
        {
            Biomass.storageOrgan *= prop.dormantBiomassDecrease.storageOrgan;
        }
    }


    if (!prop.isAnnual && consecutiveDormantDays > 30)
    {
        heatUnits = 0;
        currentWaterlogValue *= 0.5;
        previousHeatUnits = 0;
    }
}

double BasePlant::getWaterStressFactor()
{
    if (requiredWater != 0)
    {
        double temp = suppliedWater / requiredWater;
        temp = min(temp, 1.0);
        return temp;
    }
    else return 1.0;
}

double BasePlant::getWaterlogStressFactor()
{
    return pow(max(0.0, 1 - currentWaterlogValue / prop.waterTolerence()), 1/3.0);
}

double BasePlant::getNitrogenStressFactor()
{
    double stressfactor = 200.0 * nitrogen / (findOptimalNitrogenConcentration() * Biomass) - 100.0;
    stressfactor = max(stressfactor, 0.0);
    stressfactor = stressfactor / (stressfactor + exp(3.52 - 0.026 * stressfactor));
    return stressfactor;
}

double BasePlant::findHUI()
{
    return heatUnits / prop.growthStages.find(9)->second; // rbegin = back
}

double BasePlant::findPreviousHUI()
{
    return previousHeatUnits / prop.growthStages.find(9)->second; // rbegin = back
}

double BasePlant::findHUF()
{
    return prop.HeatUnitFactorNums.getValue(findHUI());
}

double BasePlant::findPreviousHUF()
{
    return prop.HeatUnitFactorNums.getValue(findPreviousHUI());
}

double BasePlant::findVPD(const double& averageTemp, const double& humidity)
{
    double Psat = 610.7*pow(10, 7.5f * averageTemp / (237.3 + averageTemp)) / 1000; // saturated vapor pressure
    return ((100 - humidity) / 100.0f) * Psat;
}

double BasePlant::calcHeight()
{
    return height;
}

double BasePlant::calcRootDepth()
{
 //   return prop.maxRootDepth * findHUI();
    return rootDepth;
}

bool BasePlant::canFlower()
{
    if (prop.isTree && getAge() < prop.yearsUntilMaturity()) // if is a tree, is under mature age.
        return false;
    if (floralInductionUnits > prop.floralInductionUnitsRequired)
        if (!prop.needsVernalization  // doesn't need vernalization
            || !prop.isObligateVernalization  // doesn't need to be fully vernalized to flower
            || vernalizationUnits > prop.vernalizationThermalUnits) // is sufficiently vernalized
            return true;
    return false;
}

double BasePlant::findPsychometricConstant(const double& temperature)
{
    return temperature > 0 ? 0.000666f : 0.000594; // if temp is larger than 0, return left, else right
}

double BasePlant::findSlopeOfSaturatedVaporCurve(const double& temperature)
{
    return 2502.66f*exp(17.2694f * temperature / (temperature + 237.3f)) / pow((temperature + 237.3f), 2);
}

double BasePlant::findNetRadiation(const double& radiation, const double& albedo)
{
    return radiation * (1.0f - albedo);
}

double BasePlant::findLatentHeat(const double& temperature)
{
    return 2.5f - 0.0022f * temperature;
}

double BasePlant::findOptimalNitrogenConcentration()
{
    // Interpolates between start and final, with it reaching final values before flowering HU
    double ratio = min(1.0, heatUnits / floweringHU);
    
    double temp =  (1 - ratio) * prop.startingNitrogenConcentration + ratio * prop.finalNitrogenConcentration;
    return temp;
}

double BasePlant::findRequiredNitrogen()
{
    double optimalNitrogen = findOptimalNitrogenConcentration() * Biomass;
    return optimalNitrogen - nitrogen;
}

double BasePlant::barometricPressure(const double& altitude)
{
    return 101.0f - 0.0115 * altitude + 0.000000544 * sqrt(altitude);
}

double BasePlant::getBiomass()
{
    return Biomass;
}

double BasePlant::getLAI()
{
    double ageMod = 1;
    double deadMod = 1;
    if (prop.isTree)
        ageMod = min(1.0, getAge() / (double)prop.yearsUntilMaturity());
    if (isDead())
        deadMod = 0;
    return LAI * ageMod * deadMod;
}

double BasePlant::getHU()
{
    return heatUnits;
}

double BasePlant::getREG()
{
    return REG;
}

double BasePlant::getNitrogen()
{
    return nitrogen;
}

double BasePlant::findFallenLeaves()
{
    return Biomass * exp(-sqrt(age / 360.0 / prop.yearsUntilMaturity())) / 1.42857; /// 1 / 1.42857 = 70% inital leaf drop. Adjust for other values.
}

std::string BasePlant::getName()
{
    return prop.name;
}

bool BasePlant::isDead()
{
    return dead;
}

bool BasePlant::isDormant()
{
    if (!prop.isAnnual && heatUnits >= maxHU)
        return true;
    if (tempstress < 0.001)
        return true;

    return false;
}

void BasePlant::createSeeds(const Month& date)
{
    if (Biomass.flowerAndfruits < prop.averageFruitWeight())
        return;

    if (!canFlower())
        return;

    int numSeeds = Biomass.flowerAndfruits / prop.averageFruitWeight() + 0.5;

    if (numSeeds == 0)
        return;

    double totalWeight = numSeeds * prop.averageFruitWeight();

    vector<double> seedWeights(numSeeds, 0);
    double sum = 0;
    for (int counter = 0; counter < numSeeds; counter++)
    {
        double added = random(0.01, 1);
        seedWeights[counter] = added;
        sum += added;
    }
    for (int counter = 0; counter < numSeeds; counter++)
    {
        seedWeights[counter] /= sum;
    }

    double extraWeight = (Biomass.flowerAndfruits - totalWeight) / (double)numSeeds;

    seedlist.reserve(numSeeds);
    for (int counter = 0; counter < numSeeds; counter++)
    {
        double fruitWeight = prop.averageFruitWeight() + extraWeight * seedWeights[counter];
        double seedWeight = fruitWeight * prop.seedRatio();
        fruitWeight -= seedWeight;

        seedlist.push_back(Seed(prop, vp, date, prop.dormancy, seedWeight, fruitWeight));
    }

    Biomass.flowerAndfruits = 0;
}

void BasePlant::reduceStandingBiomass(const WeatherData& data)
{
    double less = 0;
    double temp = data.maxTemp + data.minTemp;
    temp /= 2;
    
    less = 0.95;
    deadBiomass += Biomass * (1 - less);
    removedNitrogen += nitrogen * (1- less);

    Biomass.flowerAndfruits *= less;
    Biomass.roots *= less;
    Biomass.stem *= less;
    Biomass.storageOrgan *= less;

    if (Biomass < 0.01)
    {
        Biomass.flowerAndfruits = Biomass.roots = Biomass.stem = Biomass.storageOrgan = 0;
    }
}

int BasePlant::getAge()
{
    return int(age / 360);
}

int BasePlant::geticon()
{
    if (isDead() || getHU() < prop.growthStages[10] * .1)
        return 0;
    else if (getHU() < prop.growthStages[6])
        return vp.icon_sprout;
    else if (getHU() < prop.growthStages[7])
        return vp.icon_vegetative;
    else
        return vp.icon_mature;
}