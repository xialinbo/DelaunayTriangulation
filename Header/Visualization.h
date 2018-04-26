#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "DelaunayTriangulation.h"

using namespace std;

class Visualization
{
public:
    void ReconstructIn3D(vector<Vector3D*>, vector<vector<int>>);
};

#endif