#include <cmath>
#include "../Header/DotList.h"

DotList::DotList()
{
    _Size = 0;
    _Head = new Vector3D;
    _Tail = _Head;
    _Cur = _Head;
}

DotList::~DotList()
{
    while (_Head != _Tail)
    {
        _Cur = _Head->Next;
        delete _Head;
        _Head = _Cur;
        _Size--;
    }

    delete _Head;
    _Head = NULL;
    _Tail = NULL;
    _Cur = NULL;
}

void DotList::AddDot(Vector3D* point)
{
    *_Tail = *point;
    _Tail->Id = _Size;

    _Tail->Next = new Vector3D;
    _Tail = _Tail->Next;

    ++_Size;
}

void DotList::ResetCur()
{
    _Cur = _Head;
}

bool DotList::MoveToNext()
{
    _Cur = _Cur->Next;
    return (_Cur == _Tail) ? false : true;
}

Vector3D* DotList::GetCurDot()
{
    return _Cur;
}
