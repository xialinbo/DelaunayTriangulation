#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include "../Header/DotCloudGenerator.h"

#define PI 3.14159265

vector<Vector3D*> DotCloudGenerator::GetSphericalDots()
{
    vector<Vector3D*> dots = vector<Vector3D*>();
    ofstream file("Resource\\random_out.txt");

    int dotCount;
    cout << "Dot Count: ";
    cin >> dotCount;

    srand((unsigned)time(NULL));
    for (int i = 0; i < dotCount; i++)
    {
        Vector3D* dot = GetRandomDotEvenlyDistributed();
        file << "# " << dot->X << " " << dot->Y << " " << dot->Z << " "
            << dot->R << " " << dot->G << " " << dot->B << " " << endl;
        dots.push_back(dot);
    }

    file.close();

    return dots;
}

Vector3D* DotCloudGenerator::GetRandomDot()
{
    // use spherical coordinate
    double phi = (rand() % 360) * PI / 180;
    double theta = (rand() % 360) * PI / 180;

    double x = 1 * sin(theta) * cos(phi);
    double y = 1 * sin(theta) * sin(phi);
    double z = 1 * cos(theta);

    return new Vector3D(x, y, z);
}

Vector3D* DotCloudGenerator::GetRandomDotEvenlyDistributed()
{
    // project random dot in cartesian coordinate to unit sphere
    double x = rand() % 2000 - 1000;
    double y = rand() % 2000 - 1000;
    double z = rand() % 2000 - 1000;

    double r = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    x = x / r;
    y = y / r;
    z = z / r;

    return new Vector3D(x, y, z);
}