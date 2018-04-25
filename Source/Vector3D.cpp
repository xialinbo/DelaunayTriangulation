#include <cmath>
#include "../Header/Vector3D.h"

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    R = r;
    G = g;
    B = b;
}

Vector3D::Vector3D(double x, double y, double z, bool isAuxiliaryDot, int r, int g, int b)
{
    IsAuxiliaryDot = isAuxiliaryDot;

    X = x;
    Y = y;
    Z = z;

    R = r;
    G = g;
    B = b;
}

Vector3D::Vector3D(Vector3D* dot, double radius)
{
    Id = dot->Id;
    IsVisited = dot->IsVisited;
    IsAuxiliaryDot = dot->IsAuxiliaryDot;

    double l = sqrt(pow(dot->X, 2) + pow(dot->Y, 2) + pow(dot->Z, 2));
    X = radius * dot->X / l;
    Y = radius * dot->Y / l;
    Z = radius * dot->Z / l;

    R = dot->R;
    G = dot->G;
    B = dot->B;
}

Vector3D::~Vector3D()
{
}
