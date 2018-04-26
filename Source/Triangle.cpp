#include "../Header/Triangle.h"

Triangle::Triangle(Vector3D* v0, Vector3D* v1, Vector3D* v2)
{
    Vertex[0] = v0;
    Vertex[1] = v1;
    Vertex[2] = v2;
}

Triangle::~Triangle()
{
}

void Triangle::AssignNeighbors(Triangle* n0, Triangle* n1, Triangle* n2)
{
    Neighbor[0] = n0;
    Neighbor[1] = n1;
    Neighbor[2] = n2;
}