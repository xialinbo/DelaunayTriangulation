#ifndef VECTOR3D_H
#define VECTOR3D_H

class Vector3D
{
public:
	//coordinate
	double X, Y, Z;

	//color
	int R, G, B;

	Vector3D();
	Vector3D(double, double, double, int, int, int);
};

#endif