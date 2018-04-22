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

void Visualization::ReconstructIn3D(DotList* dots, TriangleList* mesh)
{
	vtkPoints* points = vtkPoints::New();
	vtkUnsignedCharArray* colors = vtkUnsignedCharArray::New();
	vtkCellArray* triangles = vtkCellArray::New();

	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	Vector3D point;
	vtkTriangle* triangle;

	do
	{
		point = dots->GetCurDot()->coordinate;

		points->InsertNextPoint(point.X, point.Y, point.Z);
		colors->InsertNextTuple3(point.R, point.G, point.B);
	} while (dots->MoveToNext());

	do
	{
		triangle = vtkTriangle::New();
		triangle->GetPointIds()->SetId(0, mesh->GetCurVerticesID(0));
		triangle->GetPointIds()->SetId(1, mesh->GetCurVerticesID(1));
		triangle->GetPointIds()->SetId(2, mesh->GetCurVerticesID(2));

		triangles->InsertNextCell(triangle);
	} while (mesh->MoveToNext());

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
	trianglePolyData->SetPolys(triangles);
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