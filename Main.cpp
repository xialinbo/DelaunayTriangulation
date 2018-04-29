#include <iostream>
#include <string>
#include <vector>
#include "Header/DotCloud.h"
#include "Header/Triangulation.h"
#include "Header/Visualization.h"

using namespace std;
using namespace dt;

void ClearMemory(vector<Vector3D*>&, vector<tuple<int, int, int>*>&);

int main()
{
    try
    {
        int cmd;
        cout << "Choose dot cloud source by key in [1/2]:\n 1 = Random generator\n 2 = Dot cloud file\n";
        cin >> cmd;

        vector<Vector3D*> dots
            = cmd == 1
            ? DotCloudGenerator().GetSphericalDots()
            : DotCloudReader().GetDotCloud();

        DelaunayTriangulation triangulation = DelaunayTriangulation();
        vector<tuple<int, int, int>*> mesh = triangulation.GetTriangulationResult(dots);
        cout << triangulation.GetStatistics() << endl;

        Visualization visualization = Visualization(false);
        visualization.ReconstructIn3D(dots, mesh);

        ClearMemory(dots, mesh);
    }
    catch (exception e)
    {
        cout << e.what() << endl;
        system("pause");
    }

    return 0;
}

void ClearMemory(vector<Vector3D*> &dots, vector<tuple<int, int, int>*> &mesh)
{
    vector<Vector3D*>::iterator itDots;
    for (itDots = dots.begin(); itDots != dots.end(); itDots++)
    {
        delete *itDots;
    }

    vector<tuple<int, int, int>*>::iterator itMesh;
    for (itMesh = mesh.begin(); itMesh != mesh.end(); itMesh++)
    {
        delete *itMesh;
    }
}
