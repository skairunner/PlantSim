#include "plant.h"
#include "soil.h"

#include <cmath>
#include <algorithm>

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


BasePlant::BasePlant(SoilCell* soil)
  : baseTemp(8.0f), maxLAI(3.0f), rootFraction1(0.3f), rootFraction2(0.05), maxRootDepth(500),
  maxHeight(1000), HeatUnitFactorNums(1, 17, 0.18), CO2CurveFactors(0.1f, 0.04f, 49), Biomass(0.05, 0, 0, 0), biomassToVPD(7), LAI(0), prevLAI(0),
  previousHeatUnits(0), heatUnits(0), isAnnual(true), soilPatch(soil), requiredWater(1), suppliedWater(1), height(0), waterTolerence(3)
  , currentWaterlogValue(0), nitrogen(0), floweringTempCurve(18, 24, 1), floralInductionUnitsRequired(7.0), dayNeutral(true), minimumInduction(0.1), criticalNightLength(12), longDayPlant(true)
  , floralInductionUnits(0)
  {
    growthStages[6] = 935.0;
    growthStages[9] = 1686.0f;
    growthStages[10] = 1800.0f;
    nitrogen = findRequiredNitrogen();
    nightLengthCurve = getSCurve(dayNeutral, longDayPlant, minimumInduction, criticalNightLength);

    floweringHU = growthStages[6];
    finalHU = growthStages[9];
    maxHU = growthStages[10];
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

    if (!longDayPlant)
        s *= -1;

    return SCurve(v, s, h, y);
}

void BasePlant::findREG()
  {
  REG = 1;
  REG = min(REG, getWaterStressFactor());
  REG = min(REG, getWaterlogStressFactor());
  REG = min(REG, getNitrogenStressFactor());
  }


void BasePlant::calculate(const WeatherData& data, const double& albedo, const double radiation)
  {
  ///testc 

  double heatUnitsAdded = (data.maxTemp + data.minTemp) / 2 - baseTemp;
  heatUnitsAdded = heatUnitsAdded > 0 ? heatUnitsAdded : 0;
  previousHeatUnits = heatUnits;

  

  if (heatUnitsAdded + heatUnits > maxHU && isAnnual) // If adding HU will go over the limit,
    {
    heatUnitsAdded = 0;
    heatUnits = growthStages.find(10)->second;
    }
  else
    {
    doWater(data);
    doNitrogen();
    doFloralInduction(data);
    findREG();

    heatUnits += heatUnitsAdded;
    double deltaHUF = findHUF() - findPreviousHUF();
    prevLAI = LAI;
    LAI += deltaHUF * maxLAI * (1 - exp(5.0f * (prevLAI - maxLAI))) * sqrt(REG); 
    height += deltaHUF * maxHeight * sqrt(REG);

    ///////////////////////
    double photoactiveRadiation;
    if (radiation == -1)
        double photoactiveRadiation = 0.5 * data.radiation * (1 - exp(-0.65 * LAI));
    else
        photoactiveRadiation = radiation;

    double potentialDeltaBiomass = 100 * CO2CurveFactors.getValue(data.CO2); // BE*
    potentialDeltaBiomass = potentialDeltaBiomass - biomassToVPD * (findVPD((data.maxTemp - data.minTemp)/2.0f, data.humidity) - 1); // BE'
    potentialDeltaBiomass = 0.001f * potentialDeltaBiomass * photoactiveRadiation / 10.0f; //result is in kg / m^2  

    
    //biomass += potentialDeltaBiomass * getWaterStressFactor();
    partitionBiomass(potentialDeltaBiomass);
    }
  }

double BasePlant::getRequiredWater()
{
    return requiredWater;
}

BiomassHolder BasePlant::getBiomassStruct()
{
    return Biomass;
}

void BasePlant::doWater(const WeatherData& data)
  {
  if (soilPatch)
    {
    double potentialEvaporation = 0.0032 * data.radiation / 2.456 /*MJ/kg*/ * ((data.maxTemp + data.minTemp)/2 + 17.8) * pow(data.maxTemp-data.minTemp, 0.6); // Hargreaves method.
    requiredWater = min(potentialEvaporation, potentialEvaporation * LAI);
    suppliedWater = soilPatch->requestWater(calcRootDepth(), requiredWater);

    // next add waterlog damage
    auto layers = soilPatch->getLayers();
    double depthSum = 0;
    for each (SoilLayer layer in layers)
      {
      depthSum += layer.getDepth();

      if (layer.totalWater() > layer.fieldCapacity() && calcRootDepth() > depthSum)
        {
        double tentativeValue = (layer.totalWater() - layer.fieldCapacity()) / (layer.saturatedMoisture() - layer.fieldCapacity()); 
        if (tentativeValue > 0.5f)
          currentWaterlogValue += (tentativeValue - 0.5f)/10.0;
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
    if (heatUnits > floweringHU && heatUnits < finalHU)
        floralInductionUnits += floweringTempCurve.getValue((data.maxTemp + data.minTemp) / 2.0) * nightLengthCurve.getValue(data.nightLength);

}

void BasePlant::partitionBiomass(const double dBiomass)
{
    double root, fruit, storage, shoot;    

    if (heatUnits < floweringHU)
    {
        root = 0.4 - 0.2 * heatUnits / floweringHU;
        fruit = 0 + 0.1 * heatUnits / floweringHU;
        storage = 0 + 0.1 * heatUnits / floweringHU;
        shoot = 0.6;
    }
    else if (heatUnits < finalHU) // while flowering stage...
    {
        double percentToFruiting = (heatUnits - floweringHU) / (finalHU - floweringHU);
        root = 0.2 - 0.1 * percentToFruiting;    
        
        storage = 0.1 + 0.2 * percentToFruiting;
        shoot = 0.6 - 0.3 * percentToFruiting;

        fruit = 0.1 + 0.4 * percentToFruiting;
        if (!flowering()) // if it's not able to flower, route all of the biomass into the other parts of the plant.
        {
            root += fruit / 3;
            storage += fruit / 3;
            shoot += fruit / 3;
            fruit = 0;
        }
    }
    else
    {
        root = 0.1;
        fruit = 0.5;
        storage = 0.3;
        shoot = 0.3;
    }


    Biomass.roots += dBiomass * root;
    Biomass.stem += dBiomass * shoot;
    Biomass.storageOrgan += dBiomass * storage;
    Biomass.flowerAndfruits += dBiomass * fruit;
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
  return max(0.0, 1 - currentWaterlogValue / waterTolerence);
  }

double BasePlant::getNitrogenStressFactor()
{
    double stressfactor = 200.0 * nitrogen / findRequiredNitrogen() - 100.0;
    stressfactor = max(stressfactor, 0.0);
    stressfactor = stressfactor / (stressfactor + exp(3.52 - 0.026 * stressfactor));
    return stressfactor;
}

double BasePlant::findHUI()
  {
  return heatUnits / growthStages.find(9)->second; // rbegin = back
  }

double BasePlant::findPreviousHUI()
  {
  return previousHeatUnits / growthStages.find(9)->second; // rbegin = back
  }

double BasePlant::findHUF()
  {
  return HeatUnitFactorNums.getValue(findHUI());
  }

double BasePlant::findPreviousHUF()
  {
  return HeatUnitFactorNums.getValue(findPreviousHUI());
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
  return maxRootDepth * findHUI();
  }

bool BasePlant::flowering()
{
    if (floralInductionUnits > floralInductionUnitsRequired)
        return true;
    return false;
}

double BasePlant::findPsychometricConstant(const double& temperature)
  {
  return temperature > 0 ? 0.000666f : 0.000594; // if temp is larger than 0, return left, else right
  }

double BasePlant::findSlopeOfSaturatedVaporCurve(const double& temperature)
  {
  return 2502.66f*exp( 17.2694f * temperature / (temperature + 237.3f)) / pow((temperature + 237.3f), 2);
  }

double BasePlant::findNetRadiation(const double& radiation, const double& albedo)
  {
  return radiation * (1.0f - albedo);
  }

double BasePlant::findLatentHeat(const double& temperature)
  {
  return 2.5f - 0.0022f * temperature;
  }

// currently a dummy function
double BasePlant::findOptimalNitrogenConcentration()
{
    double temp = 0.01;
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
  return LAI;
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