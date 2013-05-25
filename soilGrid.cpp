#include "soilGrid.h"
#include <stdlib.h>
#include <vector>
using namespace ALMANAC;

void SoilGrid::setPerlinProperties(noise::module::Perlin& gen, const double& freq, const double& lacturnity, const int& octave, const double&persist)
  {
  if (freq != 0)
    gen.SetFrequency(freq);
  if (lacturnity != 0)
    gen.SetLacunarity(lacturnity);
  if (octave != 0)
    gen.SetOctaveCount(octave);
  if (persist!= 0)
    gen.SetPersistence(persist);
  }

SoilGrid::SoilGrid(const int& w, const int& h, unsigned int seed)
  :width(w), height(h)
  {
  if (!seed)
    seed = rand();
  perlin.SetSeed(seed);
  sand.SetSeed(seed | 2); // Just random differences.
  clay.SetSeed(seed | 4);
  silt.SetSeed(seed | 8);

  setPerlinProperties(perlin, 0, 0, 0, 0);
  setPerlinProperties(sand, 0, 0, 0, 0);
  setPerlinProperties(clay, 0, 0, 0, 0);
  setPerlinProperties(silt, 0, 0, 0, 0);

  double zoom = 0.06f; 
  double soilZoom = 0.02f; 
  double startingZ = 0.99f;
  SoilCell buffer;
  double baseheight, total;
  const int layers = 10;
  soiltuple stBuffer;
  std::vector<soiltuple> soils(layers, soiltuple());

  for (int y = 0; y < width; y++)
    for (int x = 0; x < height; y++)
      {
      baseheight = (perlin.GetValue(x * zoom, y * zoom, 0.5) + 1) * 5000; // *5000 to get a range of roughly ten meters.
      for (int counter = layers-1; counter >= 0; counter--)
        {
        stBuffer.sand = sand.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1;
        stBuffer.clay = clay.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1;
        stBuffer.silt = silt.GetValue(x * zoom, y * zoom, startingZ - counter * soilZoom) + 1;

        total = stBuffer.sand + stBuffer.clay + stBuffer.silt; // Divide to get the percentage ratios.
        stBuffer.sand /= total; 
        stBuffer.silt /= total;
        stBuffer.clay /= total;

        soils[counter] = stBuffer;
        }
      grid.push_back(SoilFactory::createCell(baseheight, 200, soils));
      }
  }