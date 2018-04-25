#include <ctime>
#include <iostream>
#include <vector>
#include "Header/DotCloudGenerator.h"
#include "Header/DotCloudReader.h"
#include "Header/DelaunayTriangulation.h"
#include "Header/Visualization.h"

using namespace std;

int main()
{
    try
    {
        int cmd;
        cout << "Choose dot cloud source by key in [1/2]:\n 1 = Random generator\n 2 = Dot cloud file\n";
        cin >> cmd;

        vector<Vector3D*>* dots
            = cmd == 1
            ? DotCloudGenerator().GetSphericalDots()
            : DotCloudReader().GetDotCloud();

        vector<Vector3D*>* projectedDots = new vector<Vector3D*>();
        vector<Vector3D*>::iterator it;
        for (it = dots->begin(); it != dots->end(); it++)
        {
            Vector3D* projectedDot = new Vector3D((*it), 100);
            projectedDots->push_back(projectedDot);
        }

        clock_t then = clock();
        vector<Triangle*>* mesh = DelaunayTriangulation().GetTriangulationResult(projectedDots);
        cout << "Triangulation: " << clock() - then << "ms" << endl;

        then = clock();
        Visualization().ReconstructIn3D(dots, mesh);
        cout << "Render:  " << clock() - then << "ms" << endl;

        delete mesh;
        delete dots;
    }
    catch (exception e)
    {
        cout << e.what();
    }

    return 0;
}
