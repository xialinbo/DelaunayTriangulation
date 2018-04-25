#include <cmath>
#include "../Header/Vector3D.h"

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    double radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    Xp = x / radius;
    Yp = y / radius;
    Zp = z / radius;

    R = r;
    G = g;
    B = b;
}

Vector3D::Vector3D(double x, double y, double z, bool isAuxiliaryDot, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    IsAuxiliaryDot = isAuxiliaryDot;

    double radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    Xp = x / radius;
    Yp = y / radius;
    Zp = z / radius;

    R = r;
    G = g;
    B = b;
}


Vector3D::~Vector3D()
{
}
