#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
public:
    int Id;

    //coordinate in cartesian system(x, y, z)
    double X, Y, Z;

    //projected coordinate(x, y, z) in an unit sphere
    double X_Projected, Y_Projected, Z_Projected;

    //color
    int R, G, B;

    Vector3D* Next;

    Vector3D();
    Vector3D(double, double, double, int = 255, int = 248, int = 220);
};

#endif