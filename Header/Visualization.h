#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include "DataStructure.h"

namespace dt
{
    class Visualization
    {
    public:
        bool IsShowWireframe;

        Visualization(bool isShowWireframe = false);
        ~Visualization();

        void ReconstructIn3D(std::vector<Vector3D*> &dots, std::vector<std::tuple<int, int, int>*> &mesh);
    };
}

#endif