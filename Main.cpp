#include <iostream>
#include <ctime>
#include "Header/DotCloudGenerator.h"
#include "Header/DotCloudReader.h"
#include "Header/DotList.h"
#include "Header/TriangleList.h"
#include "Header/Visualization.h"

using namespace std;

int main()
{
    try
    {
        DotList* dots = new DotList();

        int inputValue;
        cout << "Choose dot cloud source by key in [1/2]:\n 1 = Random generator\n 2 = Dot cloud file\n";
        cin >> inputValue;

        if (inputValue == 1)
        {
            DotCloudGenerator* generator = new DotCloudGenerator();
            dots = generator->GetSphericalDots();
        }
        else if (inputValue == 2)
        {
            DotCloudReader* reader = new DotCloudReader();
            dots = reader->GetDotCloud();
        }

        clock_t then = clock();
        TriangleList* mesh = new TriangleList();
        do
        {
            mesh->InsertDot(dots->GetCurDot());
        } while (dots->MoveToNext());

        mesh->RemoveExtraTriangle();

        cout << "Triangulation: " << clock() - then << "ms" << endl;

        then = clock();
        Visualization* vtk = new Visualization();
        vtk->ReconstructIn3D(dots, mesh);
        cout << "Render:  " << clock() - then << "ms" << endl;

        delete dots;
        delete mesh;
    }
    catch (exception* e)
    {
        cout << e->what();
    }

    std::system("pause");
    return 0;
}
