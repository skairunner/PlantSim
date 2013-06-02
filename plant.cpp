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

double getS_CurveNum(const SCurveNumbers& scv, const double& x)
  {
  return scv.vert / (1 + exp(-scv.scale * (x - scv.horiz)));
  }



BasePlant::BasePlant()
  : baseTemp(8.0f), maxLAI(5.0f)
  {

  }

void BasePlant::calculate(const double& maxTemp, const double& minTemp,
                          const double& radiation, const double& CO2, const double& humidity)
  {
  double heatUnitsAdded = (maxTemp + minTemp) / 2 - baseTemp;
  heatUnitsAdded = heatUnitsAdded > 0 ? heatUnitsAdded : 0;
  previousHeatUnits = heatUnits;
  heatUnits += heatUnitsAdded;
  double deltaHUF = findHUF() - findPreviousHUF();
  prevLAI = LAI;
  LAI += deltaHUF * maxLAI * (1 - exp(5.0f * (prevLAI - maxLAI))) * sqrt(REG);
  ///////////////////////
  double photoactiveRadiation = 0.5 * radiation * (1 - exp(-0.65 * LAI));
  double potentialDeltaBiomass = 100 * getS_CurveNum(CO2CurveFactors, CO2); // BE*
  potentialDeltaBiomass = potentialDeltaBiomass - biomassToVPD * (findVPD((maxTemp - minTemp)/2.0f, humidity) - 1); // BE'
  potentialDeltaBiomass = 0.001 * potentialDeltaBiomass * photoactiveRadiation / 10; //result is in kg / m^2
  biomass += potentialDeltaBiomass * REG;
  }

double BasePlant::findHUI()
  {
  return heatUnits / growthStages.rbegin()->second; // rbegin = back
  }

double BasePlant::findPreviousHUI()
  {
  return previousHeatUnits / growthStages.rbegin()->second; // rbegin = back
  }

double BasePlant::findHUF()
  {
  return getS_CurveNum(HeatUnitFactorNums, findHUI());
  }

double BasePlant::findPreviousHUF()
  {
  return getS_CurveNum(HeatUnitFactorNums, findPreviousHUI());
  }

double findVPD(const double& averageTemp, const double& humidity)
  {
  double Psat = 610.7*pow(10, 7.5 * averageTemp / (237.3 + averageTemp)) / 1000; // saturated vapor pressure
  return ((100 - humidity) / 100) * Psat;
  }