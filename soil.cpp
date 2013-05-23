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
  lateral = 0;
  if ( water > fieldCapacity() * 1.001f)
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

void SoilCell::upwardsFlow()
  {
  for (auto it = Layers.begin()+1; it < Layers.end(); it++)
    {
    it->percolateUp = 0;
    it->percolateUp = (it->water - it->fieldCapacity())*(1-exp(0.05*((it-1)->water / (it-1)->fieldCapacity() - it->water / it->fieldCapacity())));
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

void SoilCell::solveAndPercolate()
  {
  // percolate ALL the layers
  for (auto it = Layers.begin(); it < Layers.end(); it++)
    it->percolateAndLateral(slope);
  upwardsFlow();
  // Move all the intracell water. Do not touch the intercell water.
  for (auto it = Layers.begin(); it < Layers.end(); it++)
    {
    if (it->isTopsoil)
      {
      (it+1)->addWater(it->percolateDown);
      it->addWater(-it->percolateDown);
      }
    else if (it->isAquifer)
      {
      it->addWater(-it->percolateDown); // Subtract the percolate down water away.
      (it-1)->addWater(it->percolateUp);
      it->addWater(-it->percolateUp);
      }
    else // If it's neither top nor bottom ...
      {
      (it+1)->addWater(it->percolateDown);
      it->addWater(-it->percolateDown);
      (it-1)->addWater(it->percolateUp);
      it->addWater(-it->percolateUp);
      }

   /* if (it != 0) //excluding the top layer
      Layers[it-1].addWater(Layers[it].percolateUp);
    if (it != Layers.size()-1) // excluding the bottom layer.
      Layers[it+1].addWater(Layers[it].percolateDown);*/

    }
  }

void SoilCell::addWater(const int& layer, const double& amount)
  {
  if (layer < Layers.size())
    Layers[layer].addWater(amount);
  }

SoilCell* SoilFactory::createTestCell()
  {
  SoilCell* output = new SoilCell();
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.back().isTopsoil = true;
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.3, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.5, 0.3, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.6, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.7, 0.1, 0.01, 0));
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.5, 0.4, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.1, 0.5, 0.1, 0));
  output->Layers.push_back(SoilLayer(0.6, 0.1, 0.01, 0));
  output->Layers.back().isAquifer = true;
  output->totalHeight = 200 * output->Layers.size();
  output->slope = 0.2;

  return output;
  }