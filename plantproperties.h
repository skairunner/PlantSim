#pragma once
#include <string>
#include <map>
#include "curves.h"

namespace ALMANAC
{
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
        SCurve HeatUnitFactorNums; // how fast it grows, shared atm
        SCurve CO2CurveFactors;  /// This one is shared for all plants.
        Parabola flowerTempCurve;
        Parabola tempCurve; //growing temp
        SCurve nightLengthCurve;

        double startingNitrogenConcentration;
        double finalNitrogenConcentration;

        BiomassHolder baseRatios;
        BiomassHolder fruitingRatios;
        BiomassHolder finalRatios;

        double minimumTemperature;
        double optimalTemperature;

        double minGerminationTemp;
        double optimalGerminationTemp;
        double germinationThermalUnits;
        double averageFruitWeight; // kg
        double seedRatio;
        int dormancy;

        double minFloweringTemp;
        double optimalFloweringTemp;
        
        double floralInductionUnitsRequired; // defaults to 7
        bool dayNeutral;
        bool longDayPlant;
        double minimumInduction;
        double criticalNightLength;

        double biomassToVPD; // bc(3), =7

        bool isAnnual; // limits HU to the maturity HUs.    



    };
}