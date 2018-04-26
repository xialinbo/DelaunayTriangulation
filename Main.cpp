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

        clock_t then = clock();
        vector<Triangle*>* mesh = DelaunayTriangulation().GetTriangulationResult(dots);
        cout << "Triangulation: " << clock() - then << "ms" << endl;

        then = clock();
        Visualization().ReconstructIn3D(dots, mesh);
        cout << "Render:  " << clock() - then << "ms" << endl;

        vector<Vector3D*>::iterator itDots;
        for (itDots = dots->begin(); itDots != dots->end(); itDots++)
        {
            delete *itDots;
        }

        vector<Triangle*>::iterator itMesh;
        for (itMesh = mesh->begin(); itMesh != mesh->end(); itMesh++)
        {
            /*for (int i = 0; i < 3; i++)
            {
                delete (*itMesh)->Vertex[i];
            }*/
            delete *itMesh;
        }

        delete mesh;
        delete dots;
    }
    catch (exception e)
    {
        cout << e.what();
    }

    return 0;
}
