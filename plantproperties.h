#pragma once
#include <string>
#include <map>
#include "curves.h"

namespace ALMANAC
{
    /// This class has intristic properties that do not change over the lifetime of a plant, eg its max LAI, growth stages, and various constants.
    struct PlantProperties
    {
        std::string name;
        double maxLAI;
        std::map<int, double> growthStages;
        double baseTemp; // ¡ÆC, for GDD calcs
        double waterTolerence;
        double maxHeight; // mm
        double maxRootDepth;
        SCurve HeatUnitFactorNums; // how fast it grows
        SCurve CO2CurveFactors;
        Parabola flowerTempCurve;
        Parabola tempCurve; //growing temp
        SCurve nightLengthCurve;

        double optimalTemperature;

        double minGerminationTemp;
        double optimalGerminationTemp;
        double germinationThermalUnits;

        double floralInductionUnitsRequired; // defaults to 7
        bool dayNeutral;
        bool longDayPlant;
        double minimumInduction;
        double criticalNightLength;

        double biomassToVPD; // bc(3)

        bool isAnnual; // limits HU to the maturity HUs.      

        double averageFruitWeight; // kg
    };
}