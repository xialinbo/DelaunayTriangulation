#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
public:
    int Id = 0;

    // coordinate
    double X, Y, Z;

    // color
    int R, G, B;

    bool IsVisited = false;
    bool IsAuxiliaryDot = false;

    Vector3D(double, double, double, int = 255, int = 248, int = 220);
    Vector3D(double, double, double, bool, int = 255, int = 248, int = 220);
    Vector3D(Vector3D*, double);
    ~Vector3D();
};

#endif