#include "plant.h"
#include <cmath>

using namespace ALMANAC;

SCurveNumbers::SCurveNumbers(const double& vertical, const double& Scale, const double& horizontal)
  : scale(Scale), horiz(horizontal), vert(vertical)
  {
  }

SCurveNumbers::SCurveNumbers()
  : scale(0), horiz(0), vert(0)
  {}

/*double SCurveNumbers::getS_CurveNum(const SCurveNumbers& scv, const double& x)
  {
  return scv.vert / (1 + exp(-scv.scale * (x - scv.horiz)));
  }*/



BasePlant::BasePlant()
  : baseTemp(8.0f), maxLAI(1.0f), rootFraction1(0.3f), rootFraction2(0.05), maxRootDepth(1000),
  maxHeight(1000), HeatUnitFactorNums(1, 17, 0.18), CO2CurveFactors(0.1f, 0.04f, 49), biomass(0), biomassToVPD(7), LAI(0), prevLAI(0),
  previousHeatUnits(0), heatUnits(0), isAnnual(true)
  {
  growthStages[9] = 1686.0f;
  growthStages[10] = 1800.0f;
  }

void BasePlant::calculate(const double& maxTemp, const double& minTemp,
                          const double& radiation, const double& CO2, const double& humidity, const double& albedo, const double& windspeed)
  {
  ///test
  REG = 1;

  double heatUnitsAdded = (maxTemp + minTemp) / 2 - baseTemp;
  heatUnitsAdded = heatUnitsAdded > 0 ? heatUnitsAdded : 0;
  previousHeatUnits = heatUnits;
  if (heatUnitsAdded + heatUnits > growthStages.find(10)->second && isAnnual) // If adding HU will go over the limit,
    {
    heatUnitsAdded = 0;
    heatUnits = growthStages.find(10)->second;
    }
  else
    {
    heatUnits += heatUnitsAdded;
    double deltaHUF = findHUF() - findPreviousHUF();
    prevLAI = LAI;
    LAI += deltaHUF * maxLAI * (1 - exp(5.0f * (prevLAI - maxLAI))) * sqrt(REG);
    ///////////////////////
    double photoactiveRadiation = 0.5 * radiation * (1 - exp(-0.65 * LAI));
    double potentialDeltaBiomass = 100 * SCurveNumbers::getS_CurveNum(CO2CurveFactors, CO2); // BE*
    potentialDeltaBiomass = potentialDeltaBiomass - biomassToVPD * (findVPD((maxTemp - minTemp)/2.0f, humidity) - 1); // BE'
    potentialDeltaBiomass = 0.001f * potentialDeltaBiomass * photoactiveRadiation / 10.0f; //result is in kg / m^2
    biomass += potentialDeltaBiomass * REG;
    }
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
  return SCurveNumbers::getS_CurveNum(HeatUnitFactorNums, findHUI());
  }

double BasePlant::findPreviousHUF()
  {
  return SCurveNumbers::getS_CurveNum(HeatUnitFactorNums, findPreviousHUI());
  }

double BasePlant::findVPD(const double& averageTemp, const double& humidity)
  {
  double Psat = 610.7*pow(10, 7.5f * averageTemp / (237.3 + averageTemp)) / 1000; // saturated vapor pressure
  return ((100 - humidity) / 100.0f) * Psat;
  }

double BasePlant::calcHeight()
  {
  return maxHeight * sqrt(findHUF());
  }

double BasePlant::calcRootDepth()
  {
  return 2.5 * maxRootDepth * findHUF();
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

double BasePlant::barometricPressure(const double& altitude)
  {
  return 101.0f - 0.0115 * altitude + 0.000000544 * sqrt(altitude);
  }

double BasePlant::getBiomass()
  {
  return biomass;
  }

double BasePlant::getLAI()
  {
  return LAI;
  }

double BasePlant::getHU()
  {
  return heatUnits;
  }