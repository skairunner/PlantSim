#pragma once
#include <string>
#include <map>
#include "curves.h"
#include "mendel.h"
#include <utility>

namespace ALMANAC
{
    using std::pair;

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

#undef RGB
    struct RGB
    {
        RGB();
        RGB(int r1, int g1, int b1);
        int r, g, b;
    };

    struct PlantVisualProperties
    {
        void randomizeLerp();
        std::string ID, name, name_plural, seedname, seedname_plural;
        int icon_sprout, icon_vegetative, icon_mature;
        bool isCover;
        bool whiteBackground; // Usually TRUE for dark-colored plants for contrast in the side bar
        RGB color1, color2;
        RGB getColor();

    protected:
        PolyGene lerp;
    };

    /// This class has intristic properties that do not change over the lifetime of a plant, eg its max LAI, growth stages, and various constants.
    struct PlantProperties
    {
        std::string name;

        double maxLAI();
        std::map<int, double> growthStages;
        double baseTemp; // ¡ÆC, for GDD calcs
        double waterTolerence();
        double maxHeight(); // mm
        double maxYearlyGrowth(); // mm, trees only
        double maxRootDepth();
        SCurve HeatUnitFactorNums; // how fast it grows, shared atm
        SCurve CO2CurveFactors;  /// This one is shared for all plants.
        Parabola flowerTempCurve;
        Parabola tempCurve; //growing temp
        Parabola vernalizationCurve;
        LeafDistribution LAIGraph;
        SCurve nightLengthCurve;

        double startingNitrogenConcentration;
        double finalNitrogenConcentration;

        BiomassHolder baseRatios;
        BiomassHolder fruitingRatios;
        BiomassHolder finalRatios;

        double dormantHeightDecrease;
        double dormantRootDecrease;
        BiomassHolder dormantBiomassDecrease;

        double minimumTemperature;
        double optimalTemperature;

        double minGerminationTemp;
        double optimalGerminationTemp;
        double germinationThermalUnits;
        double averageFruitWeight(); // kg
        double seedRatio();
        double seedViability; // 0 <= x <= 1
        int dormancy;

        double minFloweringTemp;
        double optimalFloweringTemp;

        double minVernalizationTemp; // default 0 C
        double optimalVernalizationTemp; // default 2.5 C
        double vernalizationThermalUnits; // default 45
        bool needsVernalization; // default FALSE
        bool isObligateVernalization; // default FALSE

        
        double floralInductionUnitsRequired; // defaults to 7
        bool dayNeutral;
        bool longDayPlant;
        double minimumInduction;
        double criticalNightLength;

        double biomassToVPD; // bc(3), =7

        bool isAnnual; // limits HU to the maturity HUs.    
        bool isTree;
        int yearsUntilMaturity(); // trees only
        int vegetativeMaturity(); // same
        int maxAge();
        int leafFallPeriod(); // days. over how many days the plant loses its LAI.

        PolyGene gene_maxLAI;
        PolyGene gene_waterTolerence;
        PolyGene gene_maxHeight;
        PolyGene gene_maxYearlyGrowth;
        PolyGene gene_maxRootDepth;
        PolyGene gene_averageFruitWeight;
        PolyGene gene_yearsUntilMaturity;
        PolyGene gene_vegetativeMaturity;
        PolyGene gene_maxAge;
        PolyGene gene_leafFallPeriod;
        PolyGene gene_seedRatio;
    };

    // Stores the range of variables when applicable, and can be converted to a PlantProperties with genes.
    struct MasterPlantProperties : public PlantProperties
    {
        PlantProperties convert(MendelianInheritance& mendel);




        pair<double, double> maxLAI_r;

        pair<double, double>  waterTolerence_r;
        pair<double, double>  maxHeight_r; // mm
        pair<double, double>  maxYearlyGrowth_r; // mm, trees only
        pair<double, double>  maxRootDepth_r;


        pair<double, double> averageFruitWeight_r; // kg
        pair<double, double> seedRatio_r;

        pair<int, int> yearsUntilMaturity_r; // trees only
        pair<int, int> vegetativeMaturity_r; // same
        pair<int, int> maxAge_r;
        pair<int, int> leafFallPeriod_r; // days. over how many days the plant loses its LAI.
    };
}

extern MendelianInheritance MendelModule;