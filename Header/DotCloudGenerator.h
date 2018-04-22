#ifndef DOTCLOUDGENERATOR_H
#define DOTCLOUDGENERATOR_H

#include "DotList.h"

class DotCloudGenerator
{
private:
	Vector3D GetRandomDot();
	Vector3D GetRandomDotEvenlyDistributed();
public:
	DotList* GetSphericalDots();
};

#endif