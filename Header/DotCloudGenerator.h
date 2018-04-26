#ifndef DOTCLOUDGENERATOR_H
#define DOTCLOUDGENERATOR_H

#include "Vector3D.h"

using namespace std;

class DotCloudGenerator
{
private:
    Vector3D* GetRandomDot();
    Vector3D* GetRandomDotEvenlyDistributed();
public:
    vector<Vector3D*> GetSphericalDots();
};

#endif