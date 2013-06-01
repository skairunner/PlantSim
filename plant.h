#pragma once

namespace ALMANAC
  {
  /**
  This class stores the three parameters for S-curves.
  An S-curve is in the form of y = v / (1 + exp(-s * (x - h);
  _v_ert determines the max value of the curve, or what number it approaches when x->oo
  _s_cale determines how stretched out the curve is.
  _h_orizontal determines where the 'center' of the curve is.
  **/
  struct SCurveNumbers
    {
    SCurveNumbers(const double& vertical, const double& Scale, const double& horizontal);
    double scale, horiz, vert;
    };

  double getS_CurveNum(const SCurveNumbers& scv, const double& x);

  class BasePlant
    {
    public:

    private:

    };
  }