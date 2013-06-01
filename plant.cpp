#include "plant.h"
#include <cmath>

using namespace ALMANAC;

SCurveNumbers::SCurveNumbers(const double& vertical, const double& Scale, const double& horizontal)
  : scale(Scale), horiz(horizontal), vert(vertical)
  {
  }

double getS_CurveNum(const SCurveNumbers& scv, const double& x)
  {
  return scv.vert / (1 + exp(-scv.scale * (x - scv.horiz)));
  }