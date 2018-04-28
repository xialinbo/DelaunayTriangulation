#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include "../Header/Triangulation.h"

#define INIT_VERTICES_COUNT 6
#define INIT_FACES_COUNT 8
#define VECTOR_LENGTH 1

using namespace std;
using namespace dt;

DelaunayTriangulation::DelaunayTriangulation()
{
    _ProjectedDots = new vector<Vector3D*>();
    _Mesh = new vector<Triangle*>();

    for (int i = 0; i < sizeof(_Statistics) / sizeof(long); i++)
    {
        _Statistics[i] = i;
    }
}

DelaunayTriangulation::~DelaunayTriangulation()
{
    vector<Vector3D*>::iterator itDots;
    for (itDots = _ProjectedDots->begin(); itDots != _ProjectedDots->end(); itDots++)
    {
        delete *itDots;
    }

    vector<Triangle*>::iterator itMesh;
    for (itMesh = _Mesh->begin(); itMesh != _Mesh->end(); itMesh++)
    {
        delete *itMesh;
    }

    delete _ProjectedDots;
    delete _Mesh;
}

vector<tuple<int, int, int>*> DelaunayTriangulation::GetTriangulationResult(vector<Vector3D*> &dots)
{
    _Statistics[2] = clock();

    _ProjectedDots->reserve(dots.size());

    // N random dots can form 8+(N-6)*2 triangles based on the algorithm
    _Mesh->reserve(8 + (dots.size() - 6) * 2);

    // project dots to an unit shpere for triangulation
    vector<Vector3D*>::iterator itDots;
    for (itDots = dots.begin(); itDots != dots.end(); itDots++)
    {
        Vector3D* projectedDot = new Vector3D((*itDots), VECTOR_LENGTH);
        _ProjectedDots->push_back(projectedDot);
    }

    // prepare initial convex hull with 6 vertices and 8 triangle faces
    BuildInitialHull(_ProjectedDots);

    for (itDots = _ProjectedDots->begin(); itDots != _ProjectedDots->end(); itDots++)
    {
        Vector3D* dot = *itDots;
        if (!dot->IsVisited && !dot->IsAuxiliaryDot)
        {
            InsertDot(dot);
        }
    }

    RemoveExtraTriangles();

    vector<tuple<int, int, int>*> mesh = vector<tuple<int, int, int>*>();
    vector<Triangle*>::iterator itMesh;
    for (itMesh = _Mesh->begin(); itMesh != _Mesh->end(); itMesh++)
    {
        Triangle* triangle = *itMesh;
        mesh.push_back(new tuple<int, int, int>(
            triangle->Vertex[0]->Id,
            triangle->Vertex[1]->Id,
            triangle->Vertex[2]->Id
            ));
    }

    _Statistics[3] = clock();

    return mesh;
}

void DelaunayTriangulation::BuildInitialHull(vector<Vector3D*>* dots)
{
    Vector3D* initialVertices[INIT_VERTICES_COUNT];
    Triangle* initialHullFaces[INIT_FACES_COUNT];

    Vector3D* auxiliaryDots[INIT_VERTICES_COUNT];
    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        auxiliaryDots[i] = new Vector3D(
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 0 ? VECTOR_LENGTH : 0),
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 1 ? VECTOR_LENGTH : 0),
            (i % 2 == 0 ? 1 : -1) * (i / 2 == 2 ? VECTOR_LENGTH : 0),
            true, 0, 0, 0
        );

        initialVertices[i] = auxiliaryDots[i];
    }

    double minDistance[INIT_VERTICES_COUNT] = { 0, 0, 0, 0, 0, 0 };
    vector<Vector3D*>::iterator it;
    for (it = dots->begin(); it != dots->end(); it++)
    {
        double distance[INIT_VERTICES_COUNT];
        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            distance[i] = GetDistance(auxiliaryDots[i], *it);
            if (minDistance[i] == 0 || distance[i] < minDistance[i])
            {
                minDistance[i] = distance[i];
            }
        }

        for (int i = 0; i < INIT_VERTICES_COUNT; i++)
        {
            if (minDistance[i] == distance[i] && IsMinimumValueInArray(distance, INIT_VERTICES_COUNT, i))
            {
                // if close enough, use input dot to replace auxiliary dots for initial hull construction
                initialVertices[i] = *it;
            }
        }
    }

    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        if (!initialVertices[i]->IsAuxiliaryDot)
        {
            // avoid being visited by InsertDot() again
            initialVertices[i]->IsVisited = true;
        }
    }

    int vertex0Index[] = { 0, 0, 0, 0, 1, 1, 1, 1 };
    int vertex1Index[] = { 4, 3, 5, 2, 2, 4, 3, 5 };
    int vertex2Index[] = { 2, 4, 3, 5, 4, 3, 5, 2 };

    for (int i = 0; i < INIT_FACES_COUNT; i++)
    {
        Vector3D* v0 = initialVertices[vertex0Index[i]];
        Vector3D* v1 = initialVertices[vertex1Index[i]];
        Vector3D* v2 = initialVertices[vertex2Index[i]];

        Triangle* triangle = new Triangle(v0, v1, v2);
        initialHullFaces[i] = triangle;

        _Mesh->push_back(triangle);
    }

    int neighbor0Index[] = { 1, 2, 3, 0, 7, 4, 5, 6 };
    int neighbor1Index[] = { 4, 5, 6, 7, 0, 1, 2, 3 };
    int neighbor2Index[] = { 3, 0, 1, 2, 5, 6, 7, 4 };

    for (int i = 0; i < INIT_FACES_COUNT; i++)
    {
        Triangle* n0 = initialHullFaces[neighbor0Index[i]];
        Triangle* n1 = initialHullFaces[neighbor1Index[i]];
        Triangle* n2 = initialHullFaces[neighbor2Index[i]];
        initialHullFaces[i]->AssignNeighbors(n0, n1, n2);
    }

    for (int i = 0; i < INIT_VERTICES_COUNT; i++)
    {
        dots->push_back(auxiliaryDots[i]);
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
        _Statistics[0]++;

        det_1 = GetDeterminant(triangle->Vertex[0], triangle->Vertex[1], dot);
        det_2 = GetDeterminant(triangle->Vertex[1], triangle->Vertex[2], dot);
        det_3 = GetDeterminant(triangle->Vertex[2], triangle->Vertex[0], dot);

        // if this dot projected into an existing triangle, split the existing triangle to 3 new ones
        if (det_1 >= 0 && det_2 >= 0 && det_3 >= 0)
        {
            if (!triangle->Vertex[0]->IsInSamePosition(dot)
                && !triangle->Vertex[1]->IsInSamePosition(dot)
                && !triangle->Vertex[2]->IsInSamePosition(dot))
            {
                SplitTriangle(triangle, dot);
            }

            return;
        }

        // on one side, search neighbors
        else if (det_2 >= 0 && det_3 >= 0)
            triangle = triangle->Neighbor[0];
        else if (det_1 >= 0 && det_3 >= 0)
            triangle = triangle->Neighbor[1];
        else if (det_1 >= 0 && det_2 >= 0)
            triangle = triangle->Neighbor[2];

        // cannot determine effectively 
        else if (det_1 > 0)
            triangle = triangle->Neighbor[1];
        else if (det_2 > 0)
            triangle = triangle->Neighbor[2];
        else if (det_3 > 0)
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
                break;
            }
        }

        if (isExtraTriangle)
        {
            delete *it;
            it = _Mesh->erase(it);
        }
        else
        {
            it++;
        }
    }
}

void DelaunayTriangulation::SplitTriangle(Triangle* triangle, Vector3D* dot)
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
    for (int i = 0; i < 3; i++)
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
    _Statistics[1]++;

    for (int i = 0; i < 3; i++)
    {
        if (t2->Vertex[i] == t1->Vertex[0] ||
            t2->Vertex[i] == t1->Vertex[1] ||
            t2->Vertex[i] == t1->Vertex[2])
        {
            continue;
        }

        double matrix[] = {
            t2->Vertex[i]->X - t1->Vertex[0]->X,
            t2->Vertex[i]->Y - t1->Vertex[0]->Y,
            t2->Vertex[i]->Z - t1->Vertex[0]->Z,

            t2->Vertex[i]->X - t1->Vertex[1]->X,
            t2->Vertex[i]->Y - t1->Vertex[1]->Y,
            t2->Vertex[i]->Z - t1->Vertex[1]->Z,

            t2->Vertex[i]->X - t1->Vertex[2]->X,
            t2->Vertex[i]->Y - t1->Vertex[2]->Y,
            t2->Vertex[i]->Z - t1->Vertex[2]->Z
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
    for (int j = 0; j < 3; j++)
    {
        for (int k = 0; k < 3; k++)
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
    return sqrt(pow((v1->X - v2->X), 2) +
        pow((v1->Y - v2->Y), 2) +
        pow((v1->Z - v2->Z), 2));
}

double DelaunayTriangulation::GetDeterminant(Vector3D* v1, Vector3D* v2, Vector3D* v3)
{
    double matrix[] = {
        v1->X, v1->Y, v1->Z,
        v2->X, v2->Y, v2->Z,
        v3->X, v3->Y, v3->Z
    };

    return GetDeterminant(matrix);
}

double DelaunayTriangulation::GetDeterminant(double matrix[])
{
    // inversed for left handed coordinate system
    double determinant = matrix[2] * matrix[4] * matrix[6]
        + matrix[0] * matrix[5] * matrix[7]
        + matrix[1] * matrix[3] * matrix[8]
        - matrix[0] * matrix[4] * matrix[8]
        - matrix[1] * matrix[5] * matrix[6]
        - matrix[2] * matrix[3] * matrix[7];

    // adjust result based on float number accuracy, otherwise causing deadloop
    return abs(determinant) <= DBL_EPSILON ? 0 : determinant;
}

string DelaunayTriangulation::GetStatistics()
{
    // display thousands separator
    regex regex("\\d{1,3}(?=(\\d{3})+$)");

    return "\nTriangle count: "
        + regex_replace(to_string(_Mesh->size()), regex, "$&,")
        + "\nTriangle search operations: "
        + regex_replace(to_string(_Statistics[0]), regex, "$&,")
        + "\nLocal optimizations: "
        + regex_replace(to_string(_Statistics[1]), regex, "$&,")
        + "\nTriangulation cost: "
        + to_string(_Statistics[3] - _Statistics[2])
        + "ms\n";
}