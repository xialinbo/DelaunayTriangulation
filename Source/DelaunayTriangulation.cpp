#include <vector>
#include "../Header/DelaunayTriangulation.h"

#define RADIUS 100
#define TOLERANCE 0.000000000001

DelaunayTriangulation::DelaunayTriangulation()
{
    _Mesh = vector<Triangle*>();

    // prepare initial convex hull with 6 vertices and 8 triangle faces
    Triangle* init[8];

    _AuxiliaryDot[0] = new Vector3D(1, 0, 0);
    _AuxiliaryDot[1] = new Vector3D(0, 0, 1);
    _AuxiliaryDot[2] = new Vector3D(-1, 0, 0);
    _AuxiliaryDot[3] = new Vector3D(0, 0, -1);
    _AuxiliaryDot[4] = new Vector3D(0, 1, 0);
    _AuxiliaryDot[5] = new Vector3D(0, -1, 0);

    for (int i = 0; i<8; ++i)
    {
        Vector3D* v0 = i<4 ? _AuxiliaryDot[4] : _AuxiliaryDot[5];
        Vector3D* v1 = i<4 ? _AuxiliaryDot[i] : _AuxiliaryDot[(i - 3) % 4];
        Vector3D* v2 = i<4 ? _AuxiliaryDot[(i + 1) % 4] : _AuxiliaryDot[(i - 4)];

        Triangle* triangle = new Triangle(v0, v1, v2);
        init[i] = triangle;

        _Mesh.push_back(triangle);
    }

    for (int i = 0; i < 8; ++i)
    {
        Triangle* n0 = i < 4 ? init[(i + 3) % 4] : init[(i - 3) % 4 + 4];
        Triangle* n1 = i < 4 ? init[i + 4] : init[i - 4];
        Triangle* n2 = i < 4 ? init[(i + 1) % 4] : init[(i - 1) % 4 + 4];
        init[i]->AssignNeighbors(n0, n1, n2);
    }
}

DelaunayTriangulation::~DelaunayTriangulation()
{
    for (int i = 0; i < 6; ++i)
    {
        // delete _AuxiliaryDot[i];
        _AuxiliaryDot[i] = nullptr;
    }
}

vector<Triangle*> DelaunayTriangulation::GetTriangulationResult(vector<Vector3D*> dots)
{
    vector<Vector3D*>::iterator it;
    for (it = dots.begin(); it != dots.end(); it++)
    {
        InsertDot(*it);
    }

    RemoveExtraTriangle();

    return _Mesh;
}

void DelaunayTriangulation::InsertDot(Vector3D* dot)
{
    double det_1 = 0, det_2 = 0, det_3 = 0;

    vector<Triangle*>::iterator it;
    it = _Mesh.begin();
    Triangle* triangle = *it;

    while (it != _Mesh.end())
    {
        det_1 = GetDeterminant(triangle->Vertex[0], triangle->Vertex[1], dot);
        det_2 = GetDeterminant(triangle->Vertex[1], triangle->Vertex[2], dot);
        det_3 = GetDeterminant(triangle->Vertex[2], triangle->Vertex[0], dot);

        // if this dot projected into an existing triangle, split the existing triangle to 3 new ones
        if (det_1 >= 0 && det_2 >= 0 && det_3 >= 0)
        {
            // this dot is too close to an existing dot, ignore it
            if (IsTooClose(dot, triangle->Vertex[0]) ||
                IsTooClose(dot, triangle->Vertex[1]) ||
                IsTooClose(dot, triangle->Vertex[2]))
            {
                break;
            }

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

    // point back
    for (int i = 0; i<3; ++i)
    {
        if (newTriangle1->Neighbor[1]->Neighbor[i] == triangle)
        {
            newTriangle1->Neighbor[1]->Neighbor[i] = newTriangle1;
            break;
        }
    }

    for (int i = 0; i<3; ++i)
    {
        if (newTriangle2->Neighbor[1]->Neighbor[i] == triangle)
        {
            newTriangle2->Neighbor[1]->Neighbor[i] = newTriangle2;
            break;
        }
    }

    _Mesh.push_back(newTriangle1);
    _Mesh.push_back(newTriangle2);

    // local optimization procedure
    DoLocalOptimization(triangle, triangle->Neighbor[1]);
    DoLocalOptimization(newTriangle1, newTriangle1->Neighbor[1]);
    DoLocalOptimization(newTriangle2, newTriangle2->Neighbor[1]);
}

void DelaunayTriangulation::DoLocalOptimization(Triangle* t1, Triangle* t2)
{
    for (int i = 0; i<3; ++i)
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

        // swap diagonal
        for (int j = 0; j<3; ++j)
        {
            for (int k = 0; k<3; ++k)
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

                    // point back
                    for (int l = 0; l<3; ++l)
                    {
                        if (t1->Neighbor[(j + 1) % 3]->Neighbor[l] == t2)
                        {
                            t1->Neighbor[(j + 1) % 3]->Neighbor[l] = t1;
                            break;
                        }
                    }

                    for (int l = 0; l<3; ++l)
                    {
                        if (t2->Neighbor[(k + 1) % 3]->Neighbor[l] == t1)
                        {
                            t2->Neighbor[(k + 1) % 3]->Neighbor[l] = t2;
                            break;
                        }
                    }

                    // check new neighbors
                    DoLocalOptimization(t1, t1->Neighbor[j]);
                    DoLocalOptimization(t1, t1->Neighbor[(j + 1) % 3]);
                    DoLocalOptimization(t2, t2->Neighbor[k]);
                    DoLocalOptimization(t2, t2->Neighbor[(k + 1) % 3]);

                    return;
                }
            }
        }
    }
}

void DelaunayTriangulation::RemoveExtraTriangle()
{
}

bool DelaunayTriangulation::IsTooClose(Vector3D* v1, Vector3D* v2)
{
    return (pow((v1->Xp - v2->Xp), 2) +
        pow((v1->Yp - v2->Yp), 2) +
        pow((v1->Zp - v2->Zp), 2)) <= TOLERANCE;
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


