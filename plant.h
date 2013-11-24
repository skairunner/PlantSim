#pragma once
#include "enums.h"
#include <map>
#include "Weather.h"



namespace ALMANAC
  {
  class SoilCell;
  /**
  This class stores the three parameters for S-curves.
  An S-curve is in the form of y = v / (1 + exp(-s * (x - h);
  _v_ert determines the max value of the curve, or what number it approaches when x->oo
  _s_cale determines how stretched out the curve is.
  _h_orizontal determines where the 'center' of the curve is.
  **/
  struct SCurveNumbers
    {
    SCurveNumbers(const double& vertical, const double& Scale, const double& horizontal);
    SCurveNumbers();
    double scale, horiz, vert;

    static double getS_CurveNum(const SCurveNumbers& scv, const double& x)
      {
      return scv.vert / (1 + exp(-scv.scale * (x - scv.horiz)));
      }
    };



  class BasePlant
    {
    public:
      BasePlant(SoilCell* soil = 0);
      void calculate(const WeatherData& data, const double& albedo); // plug in today's weather :v. CO2 is in ppm
      void findREG(); // probably has params
      double getHU(); // heat units
      double findHUI(); // heat unit indx, basically % grown.
      double calcHeight();
      double calcRootDepth();
      double getBiomass();
      double getLAI();
      double getRequiredWater();

      double maxLAI;
      double getREG();

      double getNitrogen();

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

      SCurveNumbers HeatUnitFactorNums;
      SCurveNumbers CO2CurveFactors;
      double biomassToVPD; // bc(3)
      double biomass; // kg
      double nitrogen; // kg/ha

      bool isAnnual; // limits HU to the maturity HUs.

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

      void doWater(const WeatherData& data);
      void doNitrogen();
    };
  }