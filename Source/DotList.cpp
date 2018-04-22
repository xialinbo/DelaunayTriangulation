#include <cmath>
#include "../Header/DotList.h"

#define RADIUS 100

DotList::DotList()
{
    size = 0;
    head = new Dot;
    head->next = NULL;
    tail = head;
    cur = head;
}

DotList::~DotList()
{
    while (head != tail)
    {
        cur = head->next;
        delete head;
        head = cur;

        size--;
    }

    delete head;
    head = NULL;
    tail = NULL;
    cur = NULL;
}

void DotList::AddDot(Vector3D point)
{
    tail->coordinate = point;
    tail->r = sqrt(pow(tail->coordinate.X, 2)
        + pow(tail->coordinate.Y, 2)
        + pow(tail->coordinate.Z, 2));

    tail->projection.X = RADIUS * (tail->coordinate.X) / tail->r;
    tail->projection.Y = RADIUS * (tail->coordinate.Y) / tail->r;
    tail->projection.Z = RADIUS * (tail->coordinate.Z) / tail->r;

    tail->id = size;

    tail->next = new Dot;
    tail = tail->next;
    tail->next = NULL;

    ++size;
}

bool DotList::MoveToNext()
{
    cur = cur->next;
    return (cur == tail) ? false : true;
}

void DotList::ResetCur()
{
    cur = head;
}

Dot* DotList::GetCurDot()
{
    return cur;
}
