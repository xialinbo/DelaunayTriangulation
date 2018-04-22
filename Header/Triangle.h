#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Dot.h"

class Triangle
{
public:
    //pointers pointing to 3 vertices
    Dot* Vertex[3];

    //pointers pointing to 3 neighbors
    Triangle* Neighbor[3];

    //pointer pointing to the next list node
    Triangle* next;
};

#endif