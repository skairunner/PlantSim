#pragma once
#include "noise.h"
#include "soilModel.h"
#include <vector>

namespace ALMANAC
  {
 
  class SoilFactory;
  class transferWater;
  class SoilCell;

  struct soiltuple
    {
    soiltuple(){}
    double sand, silt, clay;
    };

  class SoilProfile // A simplified model of soil. It consists of a vector of pair<>s, with the first number being max depth and the second member a current depth.
    {
    public:
      SoilProfile(SoilCell& sc);
      SoilProfile();
      std::vector<std::pair<double, double>> profile;
    };

  class SoilLayer // A single layer of soil.
    {
    public:
      friend class SoilFactory;
      friend class SoilCell;
      friend class transferWater;
      void addWater(const double& addwater);
      double availableWater();
      double percolationWater();
      virtual void recharge();
      double getDepth();

      double withdrawWater(const double amount);
      double withdrawWater(const double amount, const double rootdepth);

    protected:
    virtual void percolateAndLateral(const double& slope); // probably needs a parameter for the Moore direction
      SoilLayer(const double& sandi, const double& clayi, const double& silti, const double& organicMatteri, unsigned int thickness = 200);
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

  class Aquifer : public SoilLayer // specialized for things like saturation and field capacity.
    {
    public:
      virtual void recharge(); // implement a recharge function later. For now, keep the layer at 110% of FC.
    };

  class transferWater
    {
    public:
      static void transfer(SoilCell& waterOut, SoilCell& destination);
    };

  class SoilCell // a group of soil layers, most likely 10 + shallow aquifer.
    {
    friend class SoilProfile;
    public:
      friend class transferWater;
      friend class SoilFactory;
      void solveAndPercolate(); // Solve for percolation downwards and lateral flow for each layer, then put them in the respective outbound slots
      double slope; // m/m
      std::vector<double> inspectWater();
      void addWater(const int& layer, const double& amount);   
      double getTotalHeight();
      void calcTotalHeight();
      soiltuple getTopLayer();
      int getTopsoilType();
      void setMooreDirection(const int& moore);
      int getMooreDirection();
      void transferLateralWater(std::vector<SoilLayer>& OutLayers); // out of this cell into the other cell. Layer sizes MUST match, otherwise cerr.
      SoilLayer& getFront(const int& offset = 0);
      
      double requestWater(double rootDepth, double demand);

    protected:
      void upwardsFlow();
      static void adjustWater(const double& availableWater, double& lateral, double& percUp, double& percDown);
      void scale(); // make sure that the water out doesn't exceed the total available water.
      std::vector<SoilLayer> Layers;  // 0 is top, followed by 1 2 3 ...
      int MooreDirection; // One vector for the whole cell. Follows the enum {Moore}.
      double baseHeight; // height excluding soil layers.
      double totalHeight;
      int topsoilType;

    };

  class SoilFactory
    {
    public:
      static SoilCell* createTestCell();
      static SoilCell createCell(const double& baseheight, const double& depth, std::vector<soiltuple>& st);
      static int findTopsoilType(const soiltuple& st);
    private:
      noise::module::Perlin gen;
    };
  }