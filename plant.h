#pragma once
#include "enums.h"
#include <map>
#include "Weather.h"
#include "curves.h"
#include "plantproperties.h"
#include "seed.h"
#include <random>


namespace ALMANAC
{
    class SoilCell;

    class BasePlant
    {
    public:
        BasePlant(SoilCell* soil = 0);
        BasePlant(Seed seed, SoilCell* soil = 0);

        std::string getName();
        void calculate(const WeatherData& data, const double& albedo, const double radiation = -1); // plug in today's weather :v. CO2 is in ppm
        void findREG(); // probably has params
        double getHU(); // heat units
        double findHUI(); // heat unit indx, basically % grown.
        double calcHeight();
        double calcRootDepth();
        double getBiomass();
        BiomassHolder getBiomassStruct();
        double getLAI(); // strictly for accessing the LAI of the plant.
        double getRequiredWater();

        double getREG();

        double getNitrogen();

        double getInduction();
        bool canFlower();
        bool isDead();

        void createSeeds(const Month& date);

        std::vector<Seed> seedlist;

        double deadBiomass;
        double removedNitrogen;

    private:
        mt19937 rng;
        double random(double min = 0, double max = 1);

        PlantProperties prop;

        bool dead;

        double previousHeatUnits, heatUnits;
        double REG; // Stress factor. Has to be set elsewhere.
        double requiredWater;
        double suppliedWater;


        double currentWaterlogValue;

        double height;
        double rootDepth;

        double LAI, prevLAI;
        double maxBiomass;

        double floralInductionUnits;

        static SCurve getSCurve(const bool dayNeutral, const bool longDayPlant, double minInduction, const double& optimalInductionNightLength);

        BiomassHolder Biomass;  // kg        

        double nitrogen; // kg/ha

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
        double findRequiredNitrogen(); // finds the extra nitrogen the plant needs.
        double barometricPressure(const double& altitude);
        double getWaterStressFactor();
        double getWaterlogStressFactor();
        double getNitrogenStressFactor();

        double floweringHU;
        double endFloweringHU;
        double finalHU;
        double maxHU;

        void doWater(const WeatherData& data);
        void doNitrogen();
        void doFloralInduction(const WeatherData& data);
        void partitionBiomass(const double dBiomass);
        void doTempStress(const WeatherData& wd);

        void reduceStandingBiomass(const WeatherData& data); // = die off, for annual plants.
        double tempstress;
    };
}