#include <vector>
#include "../Header/DelaunayTriangulation.h"

#define INIT_VERTICES_COUNT 6

DelaunayTriangulation::DelaunayTriangulation()
{
    _Mesh = new vector<Triangle*>();
}

DelaunayTriangulation::~DelaunayTriangulation()
{
}

vector<Triangle*>* DelaunayTriangulation::GetTriangulationResult(vector<Vector3D*>* dots)
{
    BuildInitialHull(dots);

    vector<Vector3D*>::iterator it;
    for (it = dots->begin(); it != dots->end(); it++)
    {
        Vector3D* dot = *it;
        if (!dot->IsVisited)
        {
            InsertDot(dot);
        }
    }

    RemoveExtraTriangles();

    return _Mesh;
}

void DelaunayTriangulation::BuildInitialHull(vector<Vector3D*>* dots)
{
    // prepare initial convex hull with 6 vertices and 8 triangle faces
    _AuxiliaryDot[0] = new Vector3D(1, 0, 0, true, 0, 0, 0);
    _AuxiliaryDot[1] = new Vector3D(0, 0, 1, true, 0, 0, 0);
    _AuxiliaryDot[2] = new Vector3D(-1, 0, 0, true, 0, 0, 0);
    _AuxiliaryDot[3] = new Vector3D(0, 0, -1, true, 0, 0, 0);
    _AuxiliaryDot[4] = new Vector3D(0, 1, 0, true, 0, 0, 0);
    _AuxiliaryDot[5] = new Vector3D(0, -1, 0, true, 0, 0, 0);

    Vector3D* initialVertices[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    double minDistance[INIT_VERTICES_COUNT];

    vector<Vector3D*>::iterator it;
    for (it = dots->begin(); it != dots->end(); it++)
    {
        double distance[INIT_VERTICES_COUNT];
        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            distance[i] = GetDistance(_AuxiliaryDot[i], *it);
            if (minDistance[i] == 0 || distance[i] < minDistance[i])
            {
                minDistance[i] = distance[i];
            }
        }

        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            if (minDistance[i] == distance[i] && IsMinimumValueInArray(distance, INIT_VERTICES_COUNT, i))
            {
                initialVertices[i] = *it;
            }
        }
    }

    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        // use input dots to construct initial hull
        // otherwise use auxiliary dot to construct initial hull
        _InitialVertices[i] = initialVertices[i] != nullptr 
            ? initialVertices[i] : _AuxiliaryDot[i];

        _InitialVertices[i]->IsVisited = true;
    }

    Triangle* initialHullFaces[8];

    for (int i = 0; i < 8; i++)
    {
        Vector3D* v0 = i < 4 ? _InitialVertices[4] : _InitialVertices[5];
        Vector3D* v1 = i < 4 ? _InitialVertices[i] : _InitialVertices[(i - 3) % 4];
        Vector3D* v2 = i < 4 ? _InitialVertices[(i + 1) % 4] : _InitialVertices[(i - 4)];

        Triangle* triangle = new Triangle(v0, v1, v2);
        initialHullFaces[i] = triangle;

        _Mesh->push_back(triangle);
    }

    for (int i = 0; i < 8; i++)
    {
        Triangle* n0 = i < 4 ? initialHullFaces[(i + 3) % 4] : initialHullFaces[(i - 3) % 4 + 4];
        Triangle* n1 = i < 4 ? initialHullFaces[i + 4] : initialHullFaces[i - 4];
        Triangle* n2 = i < 4 ? initialHullFaces[(i + 1) % 4] : initialHullFaces[(i - 1) % 4 + 4];
        initialHullFaces[i]->AssignNeighbors(n0, n1, n2);
    }
}

void DelaunayTriangulation::InsertDot(Vector3D* dot)
{
    double det_1 = 0, det_2 = 0, det_3 = 0;

    vector<Triangle*>::iterator it;
    it = _Mesh->begin();
    Triangle* triangle = *it;

    while (it != _Mesh->end())
    {
        det_1 = GetDeterminant(triangle->Vertex[0], triangle->Vertex[1], dot);
        det_2 = GetDeterminant(triangle->Vertex[1], triangle->Vertex[2], dot);
        det_3 = GetDeterminant(triangle->Vertex[2], triangle->Vertex[0], dot);

        // if this dot projected into an existing triangle, split the existing triangle to 3 new ones
        if (det_1 >= 0 && det_2 >= 0 && det_3 >= 0)
        {
            SplitTriangle(dot, triangle);
            break;
        }

        // on one side, search neighbors
        else if (det_2 >= 0 && det_3 >= 0)
            triangle = triangle->Neighbor[0];
        else if (det_1 >= 0 && det_3 >= 0)
            triangle = triangle->Neighbor[1];
        else if (det_1 >= 0 && det_2 >= 0)
            triangle = triangle->Neighbor[2];

        // cannot determine effectively 
        else if (det_1>0)
            triangle = triangle->Neighbor[1];
        else if (det_2>0)
            triangle = triangle->Neighbor[2];
        else if (det_3>0)
            triangle = triangle->Neighbor[0];
        else
            triangle = *it++;
    }
}

void DelaunayTriangulation::RemoveExtraTriangles()
{
    vector<Triangle*>::iterator it;
    for (it = _Mesh->begin(); it != _Mesh->end();)
    {
        Triangle* triangle = *it;
        bool isExtraTriangle = false;
        for (int i = 0; i < 3; i++)
        {
            if (triangle->Vertex[i]->IsAuxiliaryDot)
            {
                isExtraTriangle = true;
                it = _Mesh->erase(it);
                break;
            }
        }

        if (!isExtraTriangle)
        {
            it++;
        }
    }
}

void DelaunayTriangulation::SplitTriangle(Vector3D* dot, Triangle* triangle)
{
    Triangle* newTriangle1 = new Triangle(dot, triangle->Vertex[1], triangle->Vertex[2]);
    Triangle* newTriangle2 = new Triangle(dot, triangle->Vertex[2], triangle->Vertex[0]);

    triangle->Vertex[2] = triangle->Vertex[1];
    triangle->Vertex[1] = triangle->Vertex[0];
    triangle->Vertex[0] = dot;

    newTriangle1->AssignNeighbors(triangle, triangle->Neighbor[1], newTriangle2);
    newTriangle2->AssignNeighbors(newTriangle1, triangle->Neighbor[2], triangle);
    triangle->AssignNeighbors(newTriangle2, triangle->Neighbor[0], newTriangle1);

    FixNeighborhood(newTriangle1->Neighbor[1], triangle, newTriangle1);
    FixNeighborhood(newTriangle2->Neighbor[1], triangle, newTriangle2);

    _Mesh->push_back(newTriangle1);
    _Mesh->push_back(newTriangle2);

    // optimize triangles according to delaunay triangulation definition
    DoLocalOptimization(triangle, triangle->Neighbor[1]);
    DoLocalOptimization(newTriangle1, newTriangle1->Neighbor[1]);
    DoLocalOptimization(newTriangle2, newTriangle2->Neighbor[1]);
}

void DelaunayTriangulation::FixNeighborhood(Triangle* target, Triangle* oldNeighbor, Triangle* newNeighbor)
{
    for (int i = 0; i<3; i++)
    {
        if (target->Neighbor[i] == oldNeighbor)
        {
            target->Neighbor[i] = newNeighbor;
            break;
        }
    }
}

void DelaunayTriangulation::DoLocalOptimization(Triangle* t1, Triangle* t2)
{
    for (int i = 0; i<3; i++)
    {
        if (t2->Vertex[i] == t1->Vertex[0] ||
            t2->Vertex[i] == t1->Vertex[1] ||
            t2->Vertex[i] == t1->Vertex[2])
        {
            continue;
        }

        double matrix[] = {
            t2->Vertex[i]->Xp - t1->Vertex[0]->Xp,
            t2->Vertex[i]->Yp - t1->Vertex[0]->Yp,
            t2->Vertex[i]->Zp - t1->Vertex[0]->Zp,

            t2->Vertex[i]->Xp - t1->Vertex[1]->Xp,
            t2->Vertex[i]->Yp - t1->Vertex[1]->Yp,
            t2->Vertex[i]->Zp - t1->Vertex[1]->Zp,

            t2->Vertex[i]->Xp - t1->Vertex[2]->Xp,
            t2->Vertex[i]->Yp - t1->Vertex[2]->Yp,
            t2->Vertex[i]->Zp - t1->Vertex[2]->Zp
        };

        if (GetDeterminant(matrix) <= 0)
        {
            // terminate after optimized
            break;
        }

        if (TrySwapDiagonal(t1, t2))
        {
            return;
        }
    }
}

bool DelaunayTriangulation::TrySwapDiagonal(Triangle * t1, Triangle * t2)
{
    for (int j = 0; j<3; j++)
    {
        for (int k = 0; k<3; k++)
        {
            if (t1->Vertex[j] != t2->Vertex[0] &&
                t1->Vertex[j] != t2->Vertex[1] &&
                t1->Vertex[j] != t2->Vertex[2] &&
                t2->Vertex[k] != t1->Vertex[0] &&
                t2->Vertex[k] != t1->Vertex[1] &&
                t2->Vertex[k] != t1->Vertex[2])
            {
                t1->Vertex[(j + 2) % 3] = t2->Vertex[k];
                t2->Vertex[(k + 2) % 3] = t1->Vertex[j];

                t1->Neighbor[(j + 1) % 3] = t2->Neighbor[(k + 2) % 3];
                t2->Neighbor[(k + 1) % 3] = t1->Neighbor[(j + 2) % 3];
                t1->Neighbor[(j + 2) % 3] = t2;
                t2->Neighbor[(k + 2) % 3] = t1;

                FixNeighborhood(t1->Neighbor[(j + 1) % 3], t2, t1);
                FixNeighborhood(t2->Neighbor[(k + 1) % 3], t1, t2);

                DoLocalOptimization(t1, t1->Neighbor[j]);
                DoLocalOptimization(t1, t1->Neighbor[(j + 1) % 3]);
                DoLocalOptimization(t2, t2->Neighbor[k]);
                DoLocalOptimization(t2, t2->Neighbor[(k + 1) % 3]);

                return true;
            }
        }
    }

    return false;
}

bool DelaunayTriangulation::IsMinimumValueInArray(double arr[], int length, int index)
{
    for (int i = 0; i < length; i++)
    {
        if (arr[i] < arr[index])
        {
            return false;
        }
    }

    return true;
}

double DelaunayTriangulation::GetDistance(Vector3D* v1, Vector3D* v2)
{
    return sqrt(pow((v1->Xp - v2->Xp), 2) +
        pow((v1->Yp - v2->Yp), 2) +
        pow((v1->Zp - v2->Zp), 2));
}

double DelaunayTriangulation::GetDeterminant(Vector3D* v1, Vector3D* v2, Vector3D* v3)
{
    double matrix[] = {
        v1->Xp, v1->Yp, v1->Zp,
        v2->Xp, v2->Yp, v2->Zp,
        v3->Xp, v3->Yp, v3->Zp
    };

    return GetDeterminant(matrix);
}

double DelaunayTriangulation::GetDeterminant(double matrix[])
{
    // inversed for left handed coordinate system
    return matrix[2] * matrix[4] * matrix[6]
        + matrix[0] * matrix[5] * matrix[7]
        + matrix[1] * matrix[3] * matrix[8]
        - matrix[0] * matrix[4] * matrix[8]
        - matrix[1] * matrix[5] * matrix[6]
        - matrix[2] * matrix[3] * matrix[7];
}