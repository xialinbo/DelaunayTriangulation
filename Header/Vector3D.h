#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
public:
    int Id;

    // coordinate in cartesian system(x, y, z)
    double X, Y, Z;

    // projected coordinate(x, y, z) in an unit sphere
    double Xp, Yp, Zp;

    // color
    int R, G, B;

    bool IsVisited;
    bool IsAuxiliaryDot;

    Vector3D(double, double, double, int = 255, int = 248, int = 220);
    Vector3D(double, double, double, bool, int = 255, int = 248, int = 220);
    ~Vector3D();
};

#endif