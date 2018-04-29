#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../Header/DotCloud.h"

using namespace std;
using namespace dt;

vector<Vector3D*> DotCloudReader::GetDotCloud()
{
    vector<Vector3D*> dots = vector<Vector3D*>();

    string filename;
    cout << "Enter name of file in resource directory: ";
    cin >> filename;
    filename = "Resource\\" + filename;
    ifstream file(filename);

    double x = 0, y = 0, z = 0;
    uint8_t red = 0, green = 0, blue = 0;
    char hex;

    //each row start with a "#"
    while (file >> hex)
    {
        file >> x >> y >> z >> red >> green >> blue;
        Vector3D* dot = new Vector3D(x, y, z, red, green, blue);
        dots.push_back(dot);
    }

    file.close();

    return dots;
}
