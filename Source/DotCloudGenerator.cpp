#include <iostream>
#include <ctime>
#include <fstream>
#include "../Header/DotCloudGenerator.h"

#define PI 3.14159265

using namespace std;

DotList* DotCloudGenerator::GetSphericalDots()
{
	DotList* dots = new DotList;
	ofstream file("Resource\\random_out.txt");

	int dotCount;
	cout << "Dot Count: ";
	cin >> dotCount;

	srand((unsigned)time(NULL));
	for (int i = 0; i < dotCount; i++)
	{
		Vector3D dot = GetRandomDotEvenlyDistributed();
		file << "# " << dot.X << " " << dot.Y << " " << dot.Z << " " << dot.R << " " << dot.G << " " << dot.B << " " << endl;
		dots->AddDot(dot);
	}

	file.close();

	return dots;
}

Vector3D DotCloudGenerator::GetRandomDot()
{
	// use spherical coordinate
	double phi = (rand() % 360) * PI / 180;
	double theta = (rand() % 360) * PI / 180;

	double x = 1 * sin(theta) * cos(phi);
	double y = 1 * sin(theta) * sin(phi);
	double z = 1 * cos(theta);

	return Vector3D(x, y, z, 255, 248, 220);
}

Vector3D DotCloudGenerator::GetRandomDotEvenlyDistributed()
{
	// project random dot in cartesian coordinate to unit sphere
	double x = rand() % 2000 - 1000;
	double y = rand() % 2000 - 1000;
	double z = rand() % 2000 - 1000;

	double r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	x = x / r;
	y = y / r;
	z = z / r;

	return Vector3D(x, y, z, 255, 248, 220);
}