#ifndef DOTLIST_H
#define DOTLIST_H

#include "Vector3D.h"

class DotList
{
private:
    int _Size;
    Vector3D* _Head;
    Vector3D* _Tail;
    Vector3D* _Cur;

public:
    DotList();
    ~DotList();
    void AddDot(Vector3D*);
    void ResetCur();
    bool MoveToNext();
    Vector3D* GetCurDot();
};

#endif