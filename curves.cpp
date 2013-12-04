#include "curves.h"

Parabola::Parabola(double root1, const double& vertexX, const double& vertexY)
{

    parallel = vertexX;
    vertical = vertexY;
    width = -1 / pow(root1 - vertexX, 2);
}

Parabola::Parabola()
{
    parallel = vertical = width = 0;
}

double Parabola::getValue(const double& x)
{
    return width * pow(x - parallel, 2) + vertical;
}




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