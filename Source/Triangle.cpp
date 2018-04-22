#include <cmath>
#include "../Header/Triangle.h"

Triangle::Triangle()
{
    for (int i = 0; i < 3; ++i)
    {
        Vertex[i] = NULL;
        Neighbor[i] = NULL;
    }

    Next = NULL;
}
