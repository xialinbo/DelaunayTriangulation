#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "DotList.h"
#include "TriangleList.h"

class Visualization
{
public:
    void ReconstructIn3D(DotList*, TriangleList*);
};

#endif