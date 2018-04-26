#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Vector3D.h"

class Triangle
{
public:
    // pointers pointing to 3 vertices
    Vector3D* Vertex[3];

    // pointers pointing to 3 neighbors
    Triangle* Neighbor[3];

    Triangle(Vector3D*, Vector3D*, Vector3D*);
    ~Triangle();

    void AssignNeighbors(Triangle*, Triangle*, Triangle*);
};

#endif