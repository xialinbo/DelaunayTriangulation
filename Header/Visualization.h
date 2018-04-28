#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "DataStructure.h"

namespace dt
{
    class Visualization
    {
    public:
        void ReconstructIn3D(std::vector<Vector3D*>&, std::vector<std::tuple<int, int, int>*>&);
    };
}

#endif