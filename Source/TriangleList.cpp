#include <iostream>
#include "../Header/TriangleList.h"

#define RADIUS 100
#define TOLERANCE 0.000000000001

TriangleList::TriangleList()
{
    _Size = 0;
    _Head = new Triangle();
    _Tail = _Head;
    _Cur = _Head;

    // prepare initial convex hull with 6 vertices and 8 triangle faces
    Triangle* octa_triangles[8];

    _HexVertices[0] = new Vector3D(1, 0, 0);
    _HexVertices[1] = new Vector3D(0, 0, 1);
    _HexVertices[2] = new Vector3D(-1, 0, 0);
    _HexVertices[3] = new Vector3D(0, 0, -1);
    _HexVertices[4] = new Vector3D(0, 1, 0);
    _HexVertices[5] = new Vector3D(0, -1, 0);

    for(int i = 0; i<8; ++i)
    {
        _Tail->Vertex[0] = i<4 ? _HexVertices[4] : _HexVertices[5];
        _Tail->Vertex[1] = i<4 ? _HexVertices[i] : _HexVertices[(i - 3) % 4];
        _Tail->Vertex[2] = i<4 ? _HexVertices[(i + 1) % 4] : _HexVertices[(i - 4)];

        octa_triangles[i] = _Tail;
        
        _Tail->Next = new Triangle;
        _Tail = _Tail->Next;

        ++_Size;
    }

    for (int i = 0; i < 8; ++i)
    {
        octa_triangles[i]->Neighbor[0] = i<4 ? octa_triangles[(i + 3) % 4] : octa_triangles[(i - 3) % 4 + 4];
        octa_triangles[i]->Neighbor[1] = i<4 ? octa_triangles[i + 4] : octa_triangles[i - 4];
        octa_triangles[i]->Neighbor[2] = i<4 ? octa_triangles[(i + 1) % 4] : octa_triangles[(i - 1) % 4 + 4];
    }
}

TriangleList::~TriangleList()
{
    while(_Head!= _Tail)
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

    for (int i = 0; i < 6; ++i)
    {
        _HexVertices[i] = NULL;
    }
}

void TriangleList::InsertDot(Vector3D* dot)
{

    double det_1 = 0, det_2 = 0, det_3 = 0;
    ResetCur();
    while(true)
    {
        det_1 = GetDeterminant(_Cur->Vertex[0], _Cur->Vertex[1], dot);
        det_2 = GetDeterminant(_Cur->Vertex[1], _Cur->Vertex[2], dot);
        det_3 = GetDeterminant(_Cur->Vertex[2], _Cur->Vertex[0], dot);

        // if this dot projected into an existing triangle, split the existing triangle to 3 new ones
        if (det_1>= 0 && det_2>= 0 && det_3>= 0)
        {
            // too close to an existing dot, ignore
            if (IsTooClose(dot, _Cur->Vertex[0]) ||
                IsTooClose(dot, _Cur->Vertex[1]) ||
                IsTooClose(dot, _Cur->Vertex[2]))
            {
                break;
            }

            SplitTriangle(dot, _Cur);
            break;
        }

        //on one side, search neighbors
        else if(det_2>= 0 && det_3>= 0)
            _Cur = _Cur->Neighbor[0];
        else if(det_1>= 0 && det_3>= 0)
            _Cur = _Cur->Neighbor[1];
        else if(det_1>= 0 && det_2>= 0)
            _Cur = _Cur->Neighbor[2];

        //cannot determine effectively 
        else if(det_1>0)
            _Cur = _Cur->Neighbor[1];
        else if(det_2>0)
            _Cur = _Cur->Neighbor[2];
        else if(det_3>0)
            _Cur = _Cur->Neighbor[0];
        else
            _Cur = _Cur->Next;
    }
}

void TriangleList::SplitTriangle(Vector3D* dot, Triangle* triangle)
{
    Triangle* newTriangle1 = new Triangle;
    Triangle* newTriangle2 = new Triangle;

    newTriangle1->Vertex[0] = dot;
    newTriangle1->Vertex[1] = triangle->Vertex[1];
    newTriangle1->Vertex[2] = triangle->Vertex[2];

    newTriangle2->Vertex[0] = dot;
    newTriangle2->Vertex[1] = triangle->Vertex[2];
    newTriangle2->Vertex[2] = triangle->Vertex[0];

    triangle->Vertex[0] = dot;
    triangle->Vertex[1] = newTriangle2->Vertex[2];
    triangle->Vertex[2] = newTriangle1->Vertex[1];

    //change neighbor pointer
    newTriangle1->Neighbor[0] = triangle;
    newTriangle1->Neighbor[1] = triangle->Neighbor[1];
    newTriangle1->Neighbor[2] = newTriangle2;

    newTriangle2->Neighbor[0] = newTriangle1;
    newTriangle2->Neighbor[1] = triangle->Neighbor[2];
    newTriangle2->Neighbor[2] = triangle;

    triangle->Neighbor[1] = triangle->Neighbor[0];
    triangle->Neighbor[0] = newTriangle2;
    triangle->Neighbor[2] = newTriangle1;

    //point back
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

    //insert and link new list node
    newTriangle2->Next = triangle->Next;
    newTriangle1->Next = newTriangle2;
    triangle->Next = newTriangle1;

    _Size += 2;

    //Local Optimization Procedure
    DoLocalOptimization(triangle, triangle->Neighbor[1]);
    DoLocalOptimization(newTriangle1, newTriangle1->Neighbor[1]);
    DoLocalOptimization(newTriangle2, newTriangle2->Neighbor[1]);
}

void TriangleList::DoLocalOptimization(Triangle* t1, Triangle* t2)
{
    for(int i = 0; i<3; ++i)
    {
        if (t2->Vertex[i] == t1->Vertex[0] ||
            t2->Vertex[i] == t1->Vertex[1] ||
            t2->Vertex[i] == t1->Vertex[2])
        {
            continue;
        }

        double matrix[] = {
            t2->Vertex[i]->X_Projected-t1->Vertex[0]->X_Projected,
            t2->Vertex[i]->Y_Projected-t1->Vertex[0]->Y_Projected,
            t2->Vertex[i]->Z_Projected-t1->Vertex[0]->Z_Projected,

            t2->Vertex[i]->X_Projected-t1->Vertex[1]->X_Projected,
            t2->Vertex[i]->Y_Projected-t1->Vertex[1]->Y_Projected,
            t2->Vertex[i]->Z_Projected-t1->Vertex[1]->Z_Projected,

            t2->Vertex[i]->X_Projected-t1->Vertex[2]->X_Projected,
            t2->Vertex[i]->Y_Projected-t1->Vertex[2]->Y_Projected,
            t2->Vertex[i]->Z_Projected-t1->Vertex[2]->Z_Projected
        };

        if (GetDeterminant(matrix) <= 0)
        {
            // already optimized
            break;
        }

        //swap diagonal
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
                    //change vertices
                    t1->Vertex[(j+2)%3] = t2->Vertex[k];
                    t2->Vertex[(k+2)%3] = t1->Vertex[j];

                    //change neighbors
                    t1->Neighbor[(j+1)%3] = t2->Neighbor[(k+2)%3];
                    t2->Neighbor[(k+1)%3] = t1->Neighbor[(j+2)%3];
                    t1->Neighbor[(j+2)%3] = t2;
                    t2->Neighbor[(k+2)%3] = t1;

                    //point back
                    for (int l = 0; l<3; ++l)
                    {
                        if (t1->Neighbor[(j+1)%3]->Neighbor[l] == t2)
                        {
                            t1->Neighbor[(j+1)%3]->Neighbor[l] = t1;
                            break;
                        }
                    }

                    for (int l = 0; l<3; ++l)
                    {
                        if (t2->Neighbor[(k+1)%3]->Neighbor[l] == t1)
                        {
                            t2->Neighbor[(k+1)%3]->Neighbor[l] = t2;
                            break;
                        }
                    }

                    //check new neighbors
                    DoLocalOptimization(t1, t1->Neighbor[j]);
                    DoLocalOptimization(t1, t1->Neighbor[(j+1)%3]);
                    DoLocalOptimization(t2, t2->Neighbor[k]);
                    DoLocalOptimization(t2, t2->Neighbor[(k+1)%3]);

                    return;
                }
            }
        }
    }    
}

void TriangleList::RemoveExtraTriangle()
{
    bool is_extra = false;
    Triangle* del = _Head->Next;

    while(del != _Tail)
    {
        for (int i = 0; i<6; ++i)
        {
            if (del->Vertex[0] == _HexVertices[i] ||
                del->Vertex[1] == _HexVertices[i] ||
                del->Vertex[2] == _HexVertices[i])
            {
                is_extra = true;
                break;
            }
        }

        if (is_extra)
        {
            _Cur->Next = del->Next;
            delete del;
            del = _Cur->Next;
            _Size--;            
        }
        else
        {
            _Cur = del;
            del = del->Next;
        }

        is_extra = false;
    }

    for (int i = 0; i<6; ++i)
    {
        if (_Head->Vertex[0] == _HexVertices[i] ||
            _Head->Vertex[1] == _HexVertices[i] ||
            _Head->Vertex[2] == _HexVertices[i])
        {
            is_extra = true;
            break;
        }
    }

    if (is_extra)
    {
        del = _Head;
        _Head = _Head->Next;

        delete del;
        del = NULL;
        _Size--;
    }
}

bool TriangleList::IsTooClose(Vector3D* v1, Vector3D* v2)
{
    return (pow((v1->X_Projected-v2->X_Projected), 2)+
            pow((v1->Y_Projected-v2->Y_Projected), 2)+
            pow((v1->Z_Projected-v2->Z_Projected), 2)) <= TOLERANCE;
}

double TriangleList::GetDeterminant(Vector3D* v1, Vector3D* v2, Vector3D* v3)
{
    double matrix[] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    matrix[0] = v1->X_Projected;
    matrix[1] = v1->Y_Projected;
    matrix[2] = v1->Z_Projected;

    matrix[3] = v2->X_Projected;
    matrix[4] = v2->Y_Projected;
    matrix[5] = v2->Z_Projected;

    matrix[6] = v3->X_Projected;
    matrix[7] = v3->Y_Projected;
    matrix[8] = v3->Z_Projected;

    return GetDeterminant(matrix);
}

double TriangleList::GetDeterminant(double matrix[])
{
    //inversed for left handed coordinate system
    return matrix[2] * matrix[4] * matrix[6]
        + matrix[0] * matrix[5] * matrix[7]
        + matrix[1] * matrix[3] * matrix[8]
        - matrix[0] * matrix[4] * matrix[8]
        - matrix[1] * matrix[5] * matrix[6]
        - matrix[2] * matrix[3] * matrix[7];
}

void TriangleList::ResetCur()
{
    _Cur = _Head;
}

bool TriangleList::MoveToNext()
{
    _Cur = _Cur->Next;
    return (_Cur == _Tail) ? false : true;
}

int TriangleList::GetCurVerticesID(int index)
{
    return _Cur->Vertex[index]->Id;
}
