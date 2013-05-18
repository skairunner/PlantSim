#pragma once
#include "noise.h"
#include "soilModel.h"
#include <vector>

namespace ALMANAC
  {
  enum MOORE {TOP = 0, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT};

  class SoilLayer // A single layer of soil.
    {
    friend class SoilFactory;
    friend class SoilCell;
    public:
      void addWater(const double& water);
    private:
    virtual void percolateAndLateral(const double& slope); // probably needs a parameter for the Moore direction
      SoilLayer(const double& sand, const double& clay, const double& silt, const double& organicMatter, unsigned int thickness = 200);
      double wiltingPoint();
      double fieldCapacity();
      double saturatedMoisture();
      double SatHydConductivity();
      double travelTime();

      double sand, clay, silt, organicMatter;
      SoilProperties properties;
      double water;
      unsigned int depth; // thickness of the soil in mm.
      double percolateDown, percolateUp, lateral;

      bool isTopsoil; // = isTopsoil
      bool isAquifer; // = isAquifer
    };

  class SoilCell // a group of soil layers, most likely 10 + shallow aquifer.
    {
    friend class SoilFactory;
    public:
      void solveAndPercolate(); // Solve for percolation downwards and lateral flow for each layer, then put them in the respective outbound slots
      int MooreDirection; // One vector for the whole cell. Follows the enum {Moore}.
      double slope; // m/m
    private:
      void upwardsFlow();
      void scale(); // make sure that the water out doesn't exceed the total available water.
      std::vector<SoilLayer> Layers;  // 0 is top, followed by 1 2 3 ...
      double totalHeight;
    };
  }