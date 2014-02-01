#pragma once
#include <cmath>
#include <exception>
#include <string>

/**
A bare-bones parabola class that gets the y-value for a certain x, and sets
the parameters appropriately for a given lower root & vertex coords.
**/
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
Similar to the Parabola class, has appropriate functions for leaf area based light competition.
Includes functions that only calculate positive area, adjust absolute height values to relative height values,
and "normalizes" the integral of the function. That is, makes the area under the curve approx. equal 1.
**/
/*
y = p(ax^2 + bx + c)
*/
struct LeafDistribution
{
public:
    LeafDistribution();
    LeafDistribution(Parabola& pb);
    LeafDistribution(bool isLine); // for a straight line.
    
    double p, a, b, c;
    double leftRoot, rightRoot;
    double getPositiveArea(double x1, double x2); // 0 <= x1, x2 <= 1
    // Automatically swaps x1 and x2 if x1 > x2. Negative area is ignored.
    double getPositiveArea(double x1, double x2, double plantHeight); 
    // this function simply divides x1, x2 by plantHeight and plugs into getPositiveArea(x1, x2)

private:
    double getIntegral(double x); // for function f(x) gets F(x), assuming C = 0
};

namespace ALMANAC
{
    // A catch-all exception intended to handle errors in the LeafDistribution class,
    // mainly less than 2 real roots.
    class AdvancedMathException : public std::exception
    {
    private:
        std::string err_msg;

    public:
        AdvancedMathException(const char *msg) : err_msg(msg) {};
        AdvancedMathException() throw() {};
        const char* what() const throw() { return this->err_msg.c_str(); };
    };
}

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
