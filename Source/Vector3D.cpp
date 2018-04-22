#include <cmath>
#include "../Header/Vector3D.h"

#define RADIUS 100

Vector3D::Vector3D()
{
    X = Y = Z = 0;
    X_Projected = Y_Projected = Z_Projected = 0;

    R = 255;
    G = 248;
    B = 220;

    Next = NULL;
}

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
    X = x;
    Y = y;
    Z = z;

    double radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    X_Projected = RADIUS * x / radius;
    Y_Projected = RADIUS * y / radius;
    Z_Projected = RADIUS * z / radius;

    R = r;
    G = g;
    B = b;

    Next = NULL;
}
