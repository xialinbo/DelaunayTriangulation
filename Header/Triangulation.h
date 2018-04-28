#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#include "DataStructure.h"

namespace dt
{
    class DelaunayTriangulation
    {
    private:
        std::vector<Vector3D*>* _ProjectedDots;
        std::vector<Triangle*>* _Mesh;

        // 0: triangle search operations
        // 1: local optimizations
        // 2: start time; 3: end time;
        long _Statistics[4];

        void BuildInitialHull(std::vector<Vector3D*>*);
        void InsertDot(Vector3D*);
        void RemoveExtraTriangles();
        void SplitTriangle(Vector3D*, Triangle*);
        void FixNeighborhood(Triangle*, Triangle*, Triangle*);
        void DoLocalOptimization(Triangle*, Triangle*);
        bool TrySwapDiagonal(Triangle*, Triangle*);
        bool IsMinimumValueInArray(double[], int, int);
        double GetDistance(Vector3D*, Vector3D*);
        double GetDeterminant(Vector3D*, Vector3D*, Vector3D*);
        double GetDeterminant(double[]);

    public:
        DelaunayTriangulation();
        ~DelaunayTriangulation();

        std::vector<std::tuple<int, int, int>*> GetTriangulationResult(std::vector<Vector3D*>&);
        std::string GetStatistics();
    };
}

#endif