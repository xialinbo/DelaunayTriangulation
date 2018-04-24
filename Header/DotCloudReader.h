#ifndef DOTCLOUDREADER_H
#define DOTCLOUDREADER_H

#include "Vector3D.h"

using namespace std;

class DotCloudReader
{
public:
    vector<Vector3D*> GetDotCloud();
};

#endif