#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector3D.h"

class Triangle
{
public:
    //pointers pointing to 3 vertices
    Vector3D* Vertex[3];

    //pointers pointing to 3 neighbors
    Triangle* Neighbor[3];

    //pointer pointing to the next list node
    Triangle* Next;

    Triangle();
};

#endif