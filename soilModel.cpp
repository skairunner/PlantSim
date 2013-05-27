#include "soilModel.h"
#include <iostream>
#include <cmath>

using namespace ALMANAC;
using namespace ALMANAC::SOIL_MODELS;

SoilProperties SoilModule::setState(const double& sand, const double& silt, const double& clay, const double& organicMatter)
  {
  internalBuffer = SoilModule1.setState(sand, clay, organicMatter);
  internalBuffer.travelTime = (internalBuffer.saturatedMoisture - internalBuffer.fieldCapacity) / internalBuffer.SatHydConductivity;
  return internalBuffer;
  }

SoilProperties SoilModule::fetch(const double& sand, const double& silt, const double& clay, const double& organicMatter)
  {
  Saxton2006 SoilModuleA;
  SoilProperties internalBuffer = SoilModuleA.setState(sand, clay, organicMatter);
  internalBuffer.travelTime = (internalBuffer.saturatedMoisture - internalBuffer.fieldCapacity) / internalBuffer.SatHydConductivity;
  return internalBuffer;
  }

SoilProperties SoilModule::getState()
  {
  return internalBuffer;
  }

Saxton2006::Saxton2006()
  :wiltingPoint(0), fieldCapacity(0), saturatedMoisture(0), SatHydConductivity(0)
  {}

SoilProperties Saxton2006::setState(const double& sand, const double& clay, const double& organicMatter)
  {
  if (sand > 1 || clay > 1 || organicMatter > 1 || sand < 0 || clay < 0 || organicMatter < 0)
    std::cerr << "Error:\nSand: " << sand << "\nClay: " << clay << "\nOM: " << organicMatter << std::endl;
  double wp1 = -0.024 * sand + 0.487 * clay + 0.006 * organicMatter + 0.005 * sand * organicMatter - 0.013 * clay * organicMatter + 0.068 * sand * clay + 0.031;
  wiltingPoint = wp1 + 0.14 * wp1 - 0.02;
  double fc1 = -0.251 * sand + 0.195 * clay + 0.011 * organicMatter + 0.006 * sand * organicMatter - 0.027 * clay * organicMatter + 0.452 * sand * clay + 0.299;
  fieldCapacity = fc1 + 1.283*fc1*fc1 - 0.374*fc1 - 0.015;
  double s33t = 0.278 * sand + 0.034 * clay + 0.022 * organicMatter - 0.018 * sand * organicMatter - 0.027 * clay * organicMatter - 0.584 * sand * clay + 0.078;
  
  saturatedMoisture = s33t + 0.636 * s33t - 0.107; 
  saturatedMoisture = saturatedMoisture + fieldCapacity - 0.097 * sand + 0.043;

  double lambda = 1/((log(1500.0f) - log(33.0f))/(log(fieldCapacity) - log(wiltingPoint)));
  SatHydConductivity = 1930 * pow((saturatedMoisture - fieldCapacity),3.0f - lambda);
  return SoilProperties(wiltingPoint, fieldCapacity, saturatedMoisture, SatHydConductivity);
  }

double Saxton2006::getFC()
  {
  return fieldCapacity;
  }

double Saxton2006::getSatHydConductivity()
  {
  return SatHydConductivity;
  }

double Saxton2006::getSatMoisture()
  {
  return saturatedMoisture;
  }

double Saxton2006::getWP()
  {
  return wiltingPoint;
  }