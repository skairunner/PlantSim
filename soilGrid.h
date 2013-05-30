#pragma once;
#include "soil.h"
#include <vector>
#include "vector3.h"
#include "noise.h"

namespace ALMANAC
  {  
  class SoilGrid // All of the soil stuffs :v
    {
    public:
      SoilGrid(const int& w, const int& h, unsigned int seed = 0);
      SoilCell get(const int& x, const int& y);
      SoilCell& ref(const int& x, const int& y);
      void set(const int& x, const int& y, const SoilCell& in);
      
      int getWidth();
      int getHeight();

      void step(); // Advance water simulation by one day

      void addRandomWater(const int& numberOf, const int& howMuch); // for testing
      void addWaterSquare(const int& x, const int& y, const int& w, const int& h, const double& howMuch);
    private:
      std::vector<SoilCell> grid;
      int width, height;
      noise::module::Perlin perlin;
      noise::module::Perlin sand, clay, silt;
      SoilCell null;

      vector3 findGradientVector(const int& x, const int& y);
      vector3 findMooreNeighborVector(const int& x, const int& y, const int& neighbor); // @ Returns 0 if not applicable.
      SoilCell* findMooreNeighbor(const int& x, const int& y, const int& neighbor);
      void setPerlinProperties(noise::module::Perlin& gen, const double& freq, const double& lacturnity, const int& octave, const double&persist);
      int findMooreDirection(vector3 input);
      void doLateralForEachCell(); // Move lateral flow stuff thingies.
      
    };

  class MapTile // The entire map
    {
    public:

    private:
    };







  }