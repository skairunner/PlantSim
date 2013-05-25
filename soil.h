#pragma once
#include "noise.h"
#include "soilModel.h"
#include <vector>

namespace ALMANAC
  {
  
  enum MOORE {TOP = 0, TOPLEFT, LEFT, BOTTOMLEFT, BOTTOM, BOTTOMRIGHT, RIGHT, TOPRIGHT};
  class SoilFactory;

  struct soiltuple
    {
    soiltuple(){}
    double sand, silt, clay;
    };

  class SoilLayer // A single layer of soil.
    {
    public:
      friend class SoilFactory;
      friend class SoilCell;
      void addWater(const double& addwater);
      double availableWater();
    protected:
    virtual void percolateAndLateral(const double& slope); // probably needs a parameter for the Moore direction
      SoilLayer(const double& sandi, const double& clayi, const double& silti, const double& organicMatteri, unsigned int thickness = 200);
      double wiltingPoint();
      double fieldCapacity();
      double saturatedMoisture();
      double SatHydConductivity();
      double travelTime();
      double percolationWater();

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
    public:
      friend class SoilFactory;
      void solveAndPercolate(); // Solve for percolation downwards and lateral flow for each layer, then put them in the respective outbound slots
      int setMoore();
      double slope; // m/m
      std::vector<double> inspectWater();
      void addWater(const int& layer, const double& amount);   
      double getTotalHeight();
      void calcTotalHeight();
      soiltuple getTopLayer();

    protected:
      void upwardsFlow();
      static void adjustWater(const double& availableWater, double& lateral, double& percUp, double& percDown);
      void scale(); // make sure that the water out doesn't exceed the total available water.
      std::vector<SoilLayer> Layers;  // 0 is top, followed by 1 2 3 ...
      int MooreDirection; // One vector for the whole cell. Follows the enum {Moore}.
      double baseHeight; // height excluding soil layers.
      double totalHeight;

    };

  class SoilFactory
    {
    public:
      static SoilCell* createTestCell();
      static SoilCell createCell(const double& baseheight, const double& depth, const std::vector<soiltuple>& st);
    private:
      noise::module::Perlin gen;
    };
  }