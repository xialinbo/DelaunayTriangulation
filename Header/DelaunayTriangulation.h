#ifndef DELAUNAYTRIANGULATION_H
#define DELAUNAYTRIANGULATION_H

#include "Triangle.h"

using namespace std;

class DelaunayTriangulation
{
private:
    vector<Triangle*> _Mesh;
    Vector3D* _AuxiliaryDot[6];

    void InsertDot(Vector3D*);
    void RemoveExtraTriangle();
    void SplitTriangle(Vector3D*, Triangle*);
    void DoLocalOptimization(Triangle*, Triangle*);
    bool IsTooClose(Vector3D*, Vector3D*);
    double GetDeterminant(Vector3D*, Vector3D*, Vector3D*);
    double GetDeterminant(double[]);

public:
    DelaunayTriangulation();
    ~DelaunayTriangulation();

    vector<Triangle*> GetTriangulationResult(vector<Vector3D*>);
};

#endif