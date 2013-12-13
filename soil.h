#pragma once
#include "noise.h"
#include "soilModel.h"
#include <vector>
#include <ostream>
#include "seed.h"
#include "plant.h"

namespace ALMANAC
{


    class SoilFactory;
    class transferWater;
    class SoilCell;
    class SoilGrid;

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
        friend class SoilGrid;
        friend class transferWater;

        void addWater(const double& addwater);
        double availableWater();
        double percolationWater();

        virtual void recharge();

        double getDepth();
        double totalWaterMoved();
        double totalWater();

        double withdrawWater(const double& amount, const int numPlants = 0);
        double withdrawWater(const double& amount, const double &rootdepth, const int numPlants = 0);

        double withdrawNitrogen(const double& amount, const double& waterUptake);
        double withdrawNitrogen(const double& amount, const double& waterUptake, const double& rootDepth);

        double nitrates; // kg / ha

        double fieldCapacity();
        double saturatedMoisture();

        double previousWater;

    protected:
        virtual void percolateAndLateral(const double& slope); // probably needs a parameter for the Moore direction
        double findMovedNitrates(const double& waterVolume);


        SoilLayer(const double& sandi, const double& clayi, const double& silti, const double& organicMatteri, unsigned int thickness = 200);
        double wiltingPoint();
        void denitrification(const double temp);
        double SatHydConductivity();
        double travelTime();
        void adjustWater();
        double getSoilWeight(); // kg / ha


        double sand, clay, silt, organicMatter; // this organic matter is the % of the soil. 
        double organicMatterWeight;  // kg / ha
        double plantmatter; // kg / m^2


        SoilProperties properties;
        double water;
        unsigned int depth; // thickness of the soil in mm.
        double percolateDown, percolateUp, lateral;
        double movedNitrates;

        bool isTopsoil; // = isTopsoil
        bool isAquifer; // = isAquifer

    private:
        static const double soilWeight; // kg / m^3
    };

    class transferWater
    {
    public:
        static void transfer(SoilCell& waterOut, SoilCell& destination);
    };

    class SoilCell // a group of soil layers, most likely 10 + shallow aquifer.
    {
    public:
        friend class transferWater;
        friend class SoilFactory;
        friend class SoilGrid;

        SoilCell();

        void addNitrogenToTop(const double& amount);
        void addWater(const int& layer, const double& amount);

        void solveAndPercolate(); // Solve for percolation downwards and lateral flow for each layer, then put them in the respective outbound slots
        void calculateNitrogen(const double temp);
        double slope; // m/m

        std::vector<double> inspectWater();
        std::vector<double> inspectNitrates();
        std::vector<SoilLayer> getLayers();

        double getTotalHeight();
        void calcTotalHeight();
        soiltuple getTopLayer();
        int getTopsoilType();
        void setMooreDirection(const int& moore);
        int getMooreDirection();
        void transferLateralWater(std::vector<SoilLayer>& OutLayers); // out of this cell into the other cell. Layer sizes MUST match, otherwise cerr.
        SoilLayer& getFront(const int& offset = 0);

        double requestWater(double rootDepth, double demand);
        double requestNitrogen(double rootDepth, double demand, double suppliedWater); // Third argument is supplied by the plant. Nitrogen amount supplied is limited by mass flow, ie water uptake.


        std::vector<BasePlant> plants;
        std::vector<Seed> seeds;


        double surfaceWater;

    protected:
        void upwardsFlow();

        void scale(); // make sure that the water out doesn't exceed the total available water.
        std::vector<SoilLayer> Layers;  // 0 is top, followed by 1 2 3 ...'
        
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

std::ostream& operator<< (std::ostream& o, ALMANAC::SoilCell& sc);