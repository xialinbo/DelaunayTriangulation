#include <cmath>
#include "../Header/Vector3D.h"

#define RADIUS 100

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    double radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    Xp = RADIUS * x / radius;
    Yp = RADIUS * y / radius;
    Zp = RADIUS * z / radius;

    R = r;
    G = g;
    B = b;
}

Vector3D::~Vector3D()
{
}

int Vector3D::GetId()
{
    static int id = 0;
    return id++;
}
