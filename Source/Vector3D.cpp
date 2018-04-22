#include "../Header/Vector3D.h"

Vector3D::Vector3D()
{
    X = 0;
    Y = 0;
    Z = 0;

    R = 255;
    G = 248;
    B = 220;
}

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    R = r;
    G = g;
    B = b;
}
