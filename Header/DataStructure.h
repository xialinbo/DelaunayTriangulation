#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

namespace dt
{
    class Vector3D
    {
    private:
        int GenerateId();
    public:
        int Id = 0;

        // coordinate
        double X, Y, Z;

        // color
        int R, G, B;

        bool IsVisited = false;
        bool IsAuxiliaryDot = false;

        Vector3D(double, double, double, int = 255, int = 248, int = 220);
        Vector3D(double, double, double, bool, int = 255, int = 248, int = 220);
        Vector3D(Vector3D*, double);
        ~Vector3D();

        bool IsInSamePosition(Vector3D*);
        std::string ToString();
    };

    class Triangle
    {
    private:
        int GenerateId();
    public:
        int Id = 0;

        // pointers pointing to 3 vertices
        Vector3D* Vertex[3];

        // pointers pointing to 3 neighbors
        Triangle* Neighbor[3];

        Triangle(Vector3D*, Vector3D*, Vector3D*);
        ~Triangle();

        void AssignNeighbors(Triangle*, Triangle*, Triangle*);
        std::string ToString();
    };
}

#endif