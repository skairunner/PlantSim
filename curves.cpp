#include "curves.h"
#include <iostream>



Parabola::Parabola(double root1, const double& vertexX, const double& vertexY)
{

    parallel = vertexX;
    vertical = vertexY;
    width = -vertexY / pow(vertexX - root1, 2);
    if (root1 - vertexX == 0)
    {
        std::cout << "Div by 0";
        throw ALMANAC::AdvancedMathException("Div by 0");
    }
        
}

Parabola::Parabola()
{
    parallel = vertical = width = 0;
}

// w * (x - p)^2 + v
double Parabola::getValue(const double& x)
{
    return width * pow(x - parallel, 2) + vertical;
}


LeafDistribution::LeafDistribution()
: p(0), a(0), b(0), c(0),
leftRoot(0), rightRoot(0)
{
}

LeafDistribution::LeafDistribution(Parabola& pb)
{
    /*   w (x - p)^2 + v 
       = w (x^2 -2px + p^2) + v 
       = w  x^2 - 2pwx + wp^2 + v  */
    a = pb.width;
    b = -2 * pb.width * pb.parallel;
    c = pb.width * pb.parallel * pb.parallel + pb.vertical;

    // roots:
    /* -v/w = (x - p)^2
       (+/-) sqrt(-v/w) + p = x
    */
    if (b * b - 4 * a * c <= 0) // standard Discriminant equation
    {
        std::cout << "Discriminant is less than/equal to 0";
        throw ALMANAC::AdvancedMathException("Discriminant is less than/equal to 0");
    }
    leftRoot = pb.parallel - sqrt(-pb.vertical / pb.width);
    rightRoot = pb.parallel + sqrt(-pb.vertical / pb.width);

    
    if (leftRoot < 0)
        leftRoot = 0;
    if (rightRoot > 1)
        rightRoot = 1;

    p = 1;

    double area = getIntegral(rightRoot) - getIntegral(leftRoot);
    p = 1 / area;
}

LeafDistribution::LeafDistribution(bool IsLine)
{
    a = 0;
    b = 0;
    c = 1;
    leftRoot = 0;
    rightRoot = 1;
    p = 1;
}

double LeafDistribution::getIntegral(double x)
{
    
    // for f(x) = p(ax^2 + bx + c)
    // F(x) = 1/3 * pax^3 + 1/2 * pbx^2 + pcx
    return 1 / 3.0 * p * a * pow(x, 3) + 0.5 * p * b * pow(x, 2) + p * c * x;    
}

double LeafDistribution::getPositiveArea(double x1, double x2)
{
    if (x1 > x2)
    {
        double temp = x2;
        x2 = x1;
        x1 = x2;
    }

    if (x2 > rightRoot)
        x2 = rightRoot;
    if (x1 < leftRoot)
        x1 = leftRoot;

    return getIntegral(x2) - getIntegral(x1);
}

double LeafDistribution::getPositiveArea(double x1, double x2, double plantHeight)
{
    return getPositiveArea(x1 / plantHeight, x2 / plantHeight);
}


//////////////////////////////
//////////////////////////////
//////////////////////////////

SCurve::SCurve(const double& vertical, const double& Scale, const double& horizontal, const double& Up)
: scale(Scale), horiz(horizontal), vert(vertical), up(Up)
{
}

SCurve::SCurve()
: scale(0), horiz(0), vert(0)
{}

double SCurve::getValue(const double& x)
{
    return up + vert / (1 + exp(-scale * (x - horiz)));
}