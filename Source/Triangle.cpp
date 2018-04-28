#include <string>
#include "../Header/DataStructure.h"

using namespace std;
using namespace dt;

Triangle::Triangle(Vector3D* v0, Vector3D* v1, Vector3D* v2)
{
    Id = GenerateId();
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

string Triangle::ToString()
{
    return "Triangle ID: " + to_string(Id) + ";\n"
        + "Vertex[0]: " + Vertex[0]->ToString()
        + "Vertex[1]: " + Vertex[1]->ToString()
        + "Vertex[2]: " + Vertex[2]->ToString()
        + "Neighbor[0] ID: " + to_string(Neighbor[0]->Id) + ", "
        + "Neighbor[1] ID: " + to_string(Neighbor[1]->Id) + ", "
        + "Neighbor[2] ID: " + to_string(Neighbor[2]->Id) + ";\n";
}

int Triangle::GenerateId()
{
    static int id = 0;
    return id++;
}