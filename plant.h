#pragma once
#include "enums.h"
#include <map>

namespace ALMANAC
  {
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
    };

  double getS_CurveNum(const SCurveNumbers& scv, const double& x);

  class BasePlant
    {
    public:
      BasePlant();
      void calculate(const double& maxTemp, const double& minTemp, const double& radiation, const double& CO2, const double& humidity); // plug in today's weather :v. CO2 is in ppm
      void findREG(); // probably has params
      double findHUI(); // heat unit indx, basically % grown.
      
    private:
      std::map<int, double> growthStages;
      const double baseTemp; // ¡ÆC
      double previousHeatUnits, heatUnits;
      double REG; // Stress factor. Has to be set elsewhere.
      double LAI, prevLAI;
      const double maxLAI;
      SCurveNumbers HeatUnitFactorNums;
      SCurveNumbers CO2CurveFactors;
      double biomassToVPD; // bc(3)
      double biomass;

      double findHUF();
      double findPreviousHUF();
      double findPreviousHUI();
      double findVPD(const double& averageTemp, const double& humidity);      
    };
  }