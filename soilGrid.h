#pragma once;
#include "soil.h"
#include <vector>
#include "vector3.h"
#include "noise.h"

namespace ALMANAC
  {
  enum SOILTYPE {stCLAY = 1, stSANDYCLAY, stSILTYCLAY, stCLAYLOAM, stSILTYCLAYLOAM, stSANDYCLAYLOAM, stLOAM, stSILTLOAM, stSANDYLOAM, stLOAMYSAND, stSAND, stSILT};
  enum SOILTYPEGROUP {stgCLAYGROUP = 1, stgSANDGROUP, stgLOAMGROUP, stgSILTGROUP};
  
  class SoilGrid // All of the soil stuffs :v
    {
    public:
      SoilGrid(const int& w, const int& h, unsigned int seed = 0);
      SoilCell get(const int& x, const int& y);
      SoilCell& ref(const int& x, const int& y);
      void set(const int& x, const int& y);
    private:
      std::vector<SoilCell> grid;
      int width, height;
      noise::module::Perlin perlin;
      noise::module::Perlin sand, clay, silt;

      vector3 slopeVector(const int& x, const int& y);
      void setPerlinProperties(noise::module::Perlin& gen, const double& freq, const double& lacturnity, const int& octave, const double&persist);
      int findMooreDirection(const vector3& input);
      
    };

  class MapTile // The entire map
    {
    public:

    private:
    };







  }