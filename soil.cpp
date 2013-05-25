#include "soil.h"
#include <cmath>

using namespace ALMANAC;
using namespace std;

SoilLayer::SoilLayer(const double& sandi, const double& clayi, const double& silti, const double& organicMatteri, unsigned int thickness)
  :sand(sandi), clay(clayi), silt(silti), organicMatter(organicMatteri), depth(thickness), isTopsoil(false), isAquifer(false)
  , water(50)
  {
  properties = SOIL_MODELS::SoilModule::fetch(sand, silt, clay, organicMatter);
  }

void SoilLayer::percolateAndLateral(const double& slope)
  {
  percolateDown = 0;
  percolateUp = 0;
  lateral = 0;
  if ( water > fieldCapacity() + 0.000001f)
    {
    double lateralFlowTime = travelTime() / slope;
    double initialperc = (water - properties.fieldCapacity)*(1-exp(-travelTime()*24-lateralFlowTime*24));// "O + QH"
    percolateDown = initialperc / (1 + 24 * travelTime()) / (24 * lateralFlowTime);
    lateral = initialperc / (1 + 24 * lateralFlowTime) / (24 * travelTime());
    }
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
  return water - wiltingPoint();
  }

double SoilLayer::percolationWater()
  {
  return (water - fieldCapacity()) > 0 ? (water - fieldCapacity()) : 0;
  }

void SoilCell::upwardsFlow()
  {
  for (auto it = Layers.begin()+1; it < Layers.end(); it++)
    {
    it->percolateUp = 0;
    if ( it->water > it->fieldCapacity() + 0.000001f)
    it->percolateUp = (it->percolationWater())*(1-exp(0.05*((it-1)->water / (it-1)->fieldCapacity() - it->water / it->fieldCapacity())));
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

void SoilCell::adjustWater(const double& availableWater, double& lateral, double& percUp, double& percDown)
  {
  double waterout = lateral + percDown + percUp;
  double mult = 0;

  if (waterout > availableWater) // if water out is larger than available water.
    {
    mult = availableWater/waterout;
    lateral *= mult;
    percUp *= mult;
    percDown *= mult;
    }
  }

void SoilCell::solveAndPercolate()
  {
  // percolate ALL the layers
  for (auto it = Layers.begin(); it < Layers.end(); it++)
    it->percolateAndLateral(slope);
  upwardsFlow();
  double throwaway = 0;
  // Move all the intracell water. Do not touch the intercell water.
  for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
    if (it->isTopsoil)
      {
      // First, check.
      adjustWater(it->percolationWater(), it->lateral, throwaway, it->percolateDown); // throwaway is 0 because topsoil has no upwards perc
      (it+1)->addWater(it->percolateDown);
      it->addWater(-it->percolateDown);
      }
    else if (it->isAquifer)
      {
      adjustWater(it->percolationWater(), it->lateral, it->percolateDown, it->percolateDown);
      it->addWater(-it->percolateDown); // Subtract the percolate down water away.
      (it-1)->addWater(it->percolateUp);
      it->addWater(-it->percolateUp);
      }
    else // If it's neither top nor bottom ...
      {
      adjustWater(it->percolationWater(), it->lateral, it->percolateDown, it->percolateDown);
      (it+1)->addWater(it->percolateDown);
      it->addWater(-it->percolateDown);
      (it-1)->addWater(it->percolateUp);
      it->addWater(-it->percolateUp);
      }
    }
  }

void SoilCell::addWater(const int& layer, const double& amount)
  {
  if (layer < Layers.size())
    Layers[layer].addWater(amount);
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

SoilCell SoilFactory::createCell(const double& baseheight, const double& depth, const std::vector<soiltuple>& st)
  {
  SoilCell output;
  for (auto it = st.begin(); it < st.end(); it++)
    {
    output.Layers.push_back(SoilLayer(it->sand, it->silt, it->clay, 0.05, depth));
    if (it == st.begin())
      output.Layers.back().isTopsoil = true;
    else if (it == st.end())
      output.Layers.back().isAquifer = true;
    }
  output.baseHeight = baseheight;
  return output;
  }