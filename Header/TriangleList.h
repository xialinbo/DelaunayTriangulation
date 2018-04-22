#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H

#include "Triangle.h"

class TriangleList
{
private:
    int _Size;
    Triangle* _Head;
    Triangle* _Tail;
    Triangle* _Cur;

    Vector3D* _HexVertices[6];

    void SplitTriangle(Vector3D*, Triangle*);
    void DoLocalOptimization(Triangle*, Triangle*);
    bool IsTooClose(Vector3D*, Vector3D*);
    double GetDeterminant(Vector3D*, Vector3D*, Vector3D*);
    double GetDeterminant(double[]);

public:
    TriangleList();
    ~TriangleList();

    void InsertDot(Vector3D*);
    void RemoveExtraTriangle();
    void ResetCur();
    bool MoveToNext();
    int GetCurVerticesID(int);
};

#endif