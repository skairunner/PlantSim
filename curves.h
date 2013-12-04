#pragma once
#include <cmath>

struct Parabola
{
public:
    Parabola(double root1, const double& vertexX, const double& vertexY);
    Parabola();
    double getValue(const double& x);
    
    double width;
    double parallel;
    double vertical;
    
};

/**
This class stores the three parameters for S-curves.
An S-curve is in the form of y = v / (1 + exp(-s * (x - h)) + l;
_v_ert determines the distance between f(x) x->infinity and x->-infinity. 
_s_cale determines how stretched out the curve is.
_h_orizontal determines where the 'center' of the curve is.
l is how much the curve was shifted vertically.
**/
struct SCurve
{
    SCurve(const double& vertical, const double& Scale, const double& horizontal, const double& Up = 0);
    SCurve();
    double scale, horiz, vert, up;

    double getValue(const double& x);
};