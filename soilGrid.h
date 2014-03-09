#pragma once;
#include "soil.h"
#include <vector>
#include "vector3.h"
#include <random>
#include "noise.h"
#include "config.h"
#ifndef STANDALONE
#include "item.h"
#endif


namespace ALMANAC
{
    struct  WeatherData;

    class SoilGrid // All of the soil stuffs :v
    {
    public:
        SoilGrid(const int& w, const int& h, unsigned int seed = 0);
        void initGridWithPlant(std::string plantID);
        SoilCell get(const int& x, const int& y);
        SoilCell& ref(const int& x, const int& y);
        void set(const int& x, const int& y, const SoilCell& in);

        int getWidth();
        int getHeight();

        void stepAll(const WeatherData& wd); // Advances soil water sim AND plant sim, but NOT surface flow sim.
        void step(const WeatherData& wd); // Advance water simulation by one day
        void stepSurfaceFlow(const WeatherData& wd, double timestep = 1); // advance surface flow by one day
        void stepPlants(const WeatherData& wd);

        void addRandomWater(const int& numberOf, const int& howMuch); // for testing
        void addWaterSquare(const int& x, const int& y, const int& w, const int& h, const double& howMuch);

        int progress;
        int maxprogress;

        double test_totalrad;
        int test_numseeds;
        int test_iterations;

        std::vector<double> radPerPlant;
        SoilCell null;

    private:
        double random(double min = 0.0, double max = 1.0);
        int random(int min, int max);
        std::vector<SoilCell> grid;
        std::mt19937 gen;
        int width, height;
        noise::module::Perlin perlin;
        noise::module::Perlin sand, clay, silt;
        noise::module::Perlin aquifer;
        

        vector3 findGradientVector(const int& x, const int& y);
        vector3 findMooreNeighborVector(const int& x, const int& y, const int& neighbor); // @ Returns 0 if not applicable.
        SoilCell* findMooreNeighbor(const int& x, const int& y, const int& neighbor);
        void setPerlinProperties(noise::module::Perlin& gen, const double& freq, const double& lacturnity, const int& octave, const double&persist);
        int findMooreDirection(vector3 input);
        void doLateralForEachCell(); // Move lateral flow stuff thingies.
        void doRunoff(); // Move water, and let some of it be absorbed
    };
}