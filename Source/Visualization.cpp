#include <vector>
#include <vtkTriangle.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include "../Header/Visualization.h"

void Visualization::ReconstructIn3D(vector<Vector3D*> dots, vector<vector<int>> mesh)
{
    vtkPoints* points = vtkPoints::New();
    vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
    vtkCellArray* cells = vtkCellArray::New();

    colors->SetNumberOfComponents(3);
    colors->SetName("Colors");

    vector<Vector3D*>::iterator itDots;
    for (itDots = dots.begin(); itDots != dots.end(); itDots++)
    {
        Vector3D* dot = *itDots;
        points->InsertNextPoint(dot->X, dot->Y, dot->Z);
        colors->InsertNextTuple3(dot->R, dot->G, dot->B);
    }

    vtkTriangle* vtkTriangle;
    vector<vector<int>>::iterator itMesh;
    for (itMesh = mesh.begin(); itMesh != mesh.end(); itMesh++)
    {
        vector<int> verticesIds = *itMesh;

        vtkTriangle = vtkTriangle::New();
        vtkTriangle->GetPointIds()->SetId(0, verticesIds[0]);
        vtkTriangle->GetPointIds()->SetId(1, verticesIds[1]);
        vtkTriangle->GetPointIds()->SetId(2, verticesIds[2]);

        cells->InsertNextCell(vtkTriangle);
    }

    vtkRenderer* renderer = vtkRenderer::New();
    vtkRenderWindow* renderWindow = vtkRenderWindow::New();
    renderWindow->AddRenderer(renderer);

    vtkRenderWindowInteractor* renderWindowInteractor =
        vtkRenderWindowInteractor::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    vtkInteractorStyleTrackballCamera* style =
        vtkInteractorStyleTrackballCamera::New();
    renderWindowInteractor->SetInteractorStyle(style);

    vtkPolyData* trianglePolyData = vtkPolyData::New();

    trianglePolyData->SetPoints(points);
    trianglePolyData->SetPolys(cells);
    trianglePolyData->GetPointData()->SetScalars(colors);

    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputData(trianglePolyData);

    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    //actor->GetProperty()->SetRepresentationToWireframe();

    renderer->AddActor(actor);

    renderer->SetBackground(0.1, 0.2, 0.4);
    renderWindow->SetSize(1280, 960);

    renderWindow->Render();
    renderWindowInteractor->Start();
}