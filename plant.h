#pragma once
#include "enums.h"
#include <map>
#include "Weather.h"
#include "curves.h"


namespace ALMANAC
  {
  class SoilCell;
  

  struct BiomassHolder
  {
      BiomassHolder();
      BiomassHolder(double Stem, double Roots, double Storage, double Fruits);
      double stem;
      double roots;
      double storageOrgan;
      double flowerAndfruits;
      double getBiomass() const;
      operator double() const;
  };



  class BasePlant
    {
    public:
      BasePlant(SoilCell* soil = 0);
      void calculate(const WeatherData& data, const double& albedo, const double radiation = -1); // plug in today's weather :v. CO2 is in ppm
      void findREG(); // probably has params
      double getHU(); // heat units
      double findHUI(); // heat unit indx, basically % grown.
      double calcHeight();
      double calcRootDepth();
      double getBiomass();
      BiomassHolder getBiomassStruct();
      double getLAI();
      double getRequiredWater();

      double maxLAI;
      double getREG();

      double getNitrogen();
      bool flowering();

    private:
      std::map<int, double> growthStages;
      double baseTemp; // ¡ÆC
      double previousHeatUnits, heatUnits;
      double REG; // Stress factor. Has to be set elsewhere.
      double requiredWater;
      double suppliedWater;

      double waterTolerence;
      double currentWaterlogValue;

      double height;
      double maxHeight; //mm

      double LAI, prevLAI;      
           
      double rootFraction1, rootFraction2; // fraction of root weight; 1 is at germination, 2 at maturity
      double maxRootDepth; //mm     

      SCurve HeatUnitFactorNums;
      SCurve CO2CurveFactors;

      Parabola floweringTempCurve;
      double floralInductionUnitsRequired; // default 7
      double floralInductionUnits;
      bool dayNeutral;
      bool longDayPlant; // long day/short night plant if true, short day / long night plant if false.
      double minimumInduction;
      double criticalNightLength;
      SCurve nightLengthCurve;
      static SCurve getSCurve(const bool dayNeutral, const bool longDayPlant, double minInduction, const double& optimalInductionNightLength);

      double biomassToVPD; // bc(3)
      BiomassHolder Biomass;  // kg

      double nitrogen; // kg/ha

      bool isAnnual; // limits HU to the maturity HUs.
      double flowerFactor; // % of flowering successfully done.

      SoilCell* soilPatch;

      double findHUF();
      double findPreviousHUF();
      double findPreviousHUI();
      double findVPD(const double& averageTemp, const double& humidity);      
      double findPsychometricConstant(const double& temperature = 2.0f); // AKA 'gamma'
      double findSlopeOfSaturatedVaporCurve(const double& temperature);
      double findNetRadiation(const double& radiation, const double& albedo); //aka h(0)
      double findLatentHeat(const double& temperature);
      double findOptimalNitrogenConcentration(); // currently a dummy function
      double findRequiredNitrogen();
      double barometricPressure(const double& altitude);
      double getWaterStressFactor();
      double getWaterlogStressFactor();
      double getNitrogenStressFactor(); 

      double floweringHU;
      double finalHU;
      double maxHU;

      void doWater(const WeatherData& data);
      void doNitrogen();
      void doFloralInduction(const WeatherData& data);
      void partitionBiomass(const double dBiomass);
    };
  }