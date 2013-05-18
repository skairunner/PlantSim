#include "soilLayer.h"
#include <cmath>

using namespace ALMANAC;

void SoilLayer::percolateAndLateral(const double& slope)
  {
  percolateDown = 0;
  if ( water > properties.fieldCapacity * 1.001f)
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

void SoilCell::upwardsFlow()
  {
  for (auto it = Layers.begin()+1; it < Layers.end(); it++)
    {
    it->percolateUp = (it->water - it->fieldCapacity())*(1-exp(0.05*((it-1)->water / (it-1)->fieldCapacity() - it->water / it->fieldCapacity())));
    } // for each layer, run the algorithm. Exclude the top layer.
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