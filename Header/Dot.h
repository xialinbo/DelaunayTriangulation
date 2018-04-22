#ifndef DOT_H
#define DOT_H

#include "Vector3D.h"

class Dot
{
public:
    //coordinate in cartesian system(x, y, z)
    Vector3D coordinate;

    //projected coordinate(x, y, z) in an unit sphere
    Vector3D projection;

    double r;
    int id;
    Dot* next;
};

#endif