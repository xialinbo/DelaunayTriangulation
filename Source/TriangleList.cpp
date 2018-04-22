#include <iostream>
#include "../Header/TriangleList.h"

#define RADIUS 100
#define TOLERANCE 0.000000000001

TriangleList::TriangleList()
{
	size = 0;
	head = new Triangle;

	for (int i = 0; i < 3; ++i)
	{
		head->Vertex[i] = NULL;
		head->Neighbor[i] = NULL;
	}

	head->next = NULL;

	tail = head;
	cur = head;

	octahedron = new DotList();
	Triangle* octa_triangles[8];

	octahedron->AddDot(Vector3D(RADIUS, 0, 0, 105, 105, 255));
	octahedron->AddDot(Vector3D(0, 0, RADIUS, 105, 105, 255));
	octahedron->AddDot(Vector3D(-RADIUS, 0, 0, 105, 105, 255));
	octahedron->AddDot(Vector3D(0, 0, -RADIUS, 105, 105, 255));
	octahedron->AddDot(Vector3D(0, RADIUS, 0, 105, 105, 255));
	octahedron->AddDot(Vector3D(0, -RADIUS, 0, 105, 105, 255));

	for(int i = 0; i < 6; ++i)
	{
		hex_vertices[i] = octahedron->GetCurDot();
		octahedron->MoveToNext();
	}

	for(int i = 0; i<4; ++i)
	{
		tail->Vertex[0] = hex_vertices[4];
		tail->Vertex[1] = hex_vertices[i];
		tail->Vertex[2] = hex_vertices[(i+1)%4];

		octa_triangles[i] = tail;

		tail->next = new Triangle;
		tail = tail->next;

		for (int i = 0; i<3; ++i)
		{
			tail->Vertex[i] = NULL;
			tail->Neighbor[i] = NULL;
		}

		tail->next = NULL;

		++size;
	}

	for(int i = 4; i<8; ++i)
	{
		tail->Vertex[0] = hex_vertices[5];
		tail->Vertex[1] = hex_vertices[(i-3)%4];
		tail->Vertex[2] = hex_vertices[(i-4)];

		octa_triangles[i] = tail;

		tail->next = new Triangle;
		tail = tail->next;

		for (int i = 0; i<3; ++i)
		{
			tail->Vertex[i] = NULL;
			tail->Neighbor[i] = NULL;
		}

		tail->next = NULL;

		++size;
	}

	for(int i = 0; i<4; ++i)
	{
		octa_triangles[i]->Neighbor[0] = octa_triangles[(i+3)%4];
		octa_triangles[i]->Neighbor[1] = octa_triangles[i+4];
		octa_triangles[i]->Neighbor[2] = octa_triangles[(i+1)%4];
	}

	for(int i = 0; i<4; ++i)
	{
		octa_triangles[i+4]->Neighbor[0] = octa_triangles[(i+1)%4+4];
		octa_triangles[i+4]->Neighbor[1] = octa_triangles[i];
		octa_triangles[i+4]->Neighbor[2] = octa_triangles[(i+3)%4+4];
	}
}

TriangleList::~TriangleList()
{
	while(head!= tail)
	{
		cur = head->next;
		delete head;
		head = cur;

		size--;
	}
		
	delete head;
	head = NULL;
	tail = NULL;
	cur = NULL;

	delete octahedron;
	octahedron = NULL;
	for (int i = 0; i < 6; ++i)
	{
		hex_vertices[i] = NULL;
	}
}

void TriangleList::DoTriangulation(DotList* dots)
{
	do
	{
		InsertDot(dots->GetCurDot());
	} while (dots->MoveToNext());

	dots->ResetCur();

	this->RemoveExtraTriangle();
}

void TriangleList::InsertDot(Dot* dot)
{
	double matrix[] = {0, 0, 0,
					  0, 0, 0,
					  0, 0, 0
	};

	double det_1 = 0, det_2 = 0, det_3 = 0;
	bool insert_not_done = true;
	int index = 0;

	while(insert_not_done)
	{
		//assign coordinate value
		matrix[0] = cur->Vertex[0]->projection.X;
		matrix[1] = cur->Vertex[0]->projection.Y;
		matrix[2] = cur->Vertex[0]->projection.Z;
		matrix[3] = cur->Vertex[1]->projection.X;
		matrix[4] = cur->Vertex[1]->projection.Y;
		matrix[5] = cur->Vertex[1]->projection.Z;
		matrix[6] = dot->projection.X;
		matrix[7] = dot->projection.Y;
		matrix[8] = dot->projection.Z;

		//get the first determinant
		det_1 = this->GetDeterminant(matrix);

		//assign coordinate value
		matrix[0] = cur->Vertex[1]->projection.X;
		matrix[1] = cur->Vertex[1]->projection.Y;
		matrix[2] = cur->Vertex[1]->projection.Z;

		matrix[3] = cur->Vertex[2]->projection.X;
		matrix[4] = cur->Vertex[2]->projection.Y;
		matrix[5] = cur->Vertex[2]->projection.Z;

		//get the second determinant
		det_2 = this->GetDeterminant(matrix);

		//assign coordinate value
		matrix[0] = cur->Vertex[2]->projection.X;
		matrix[1] = cur->Vertex[2]->projection.Y;
		matrix[2] = cur->Vertex[2]->projection.Z;

		matrix[3] = cur->Vertex[0]->projection.X;
		matrix[4] = cur->Vertex[0]->projection.Y;
		matrix[5] = cur->Vertex[0]->projection.Z;

		//get the third determinant
		det_3 = this->GetDeterminant(matrix);

		//determinant conditions
		//inside the triangle, create new triangular mesh
		if (det_1>= 0 && det_2>= 0 && det_3>= 0)
		{
			//if not too close
			if (this->NotTooClose(dot, cur->Vertex[0])&&
				this->NotTooClose(dot, cur->Vertex[1])&&
				this->NotTooClose(dot, cur->Vertex[2]))
			{
				//create 2 new list node
				Triangle* temp_1 = new Triangle;
				Triangle* temp_2 = new Triangle;

				//assign vertex pointer
				temp_1->Vertex[0] = dot;
				temp_1->Vertex[1] = cur->Vertex[1];
				temp_1->Vertex[2] = cur->Vertex[2];

				temp_2->Vertex[0] = dot;
				temp_2->Vertex[1] = cur->Vertex[2];
				temp_2->Vertex[2] = cur->Vertex[0];

				cur->Vertex[0] = dot;
				cur->Vertex[1] = temp_2->Vertex[2];
				cur->Vertex[2] = temp_1->Vertex[1];

				//change neighbor pointer
				temp_1->Neighbor[0] = cur;
				temp_1->Neighbor[1] = cur->Neighbor[1];
				temp_1->Neighbor[2] = temp_2;

				temp_2->Neighbor[0] = temp_1;
				temp_2->Neighbor[1] = cur->Neighbor[2];
				temp_2->Neighbor[2] = cur;

				cur->Neighbor[1] = cur->Neighbor[0];
				cur->Neighbor[0] = temp_2;
				cur->Neighbor[2] = temp_1;

				//pointer outside reoriented
				//temp_1
				for (int i = 0; i<3; ++i)
				{
					if (temp_1->Neighbor[1]->Neighbor[i] == cur)
					{
						temp_1->Neighbor[1]->Neighbor[i] = temp_1;
						break;
					}
				}
			
				//temp_2
				for (int i = 0; i<3; ++i)
				{
					if (temp_2->Neighbor[1]->Neighbor[i] == cur)
					{
						temp_2->Neighbor[1]->Neighbor[i] = temp_2;
						break;
					}
				}

				//insert and link new list node
				temp_2->next = cur->next;
				temp_1->next = temp_2;
				cur->next = temp_1;

				size+= 2;

				//Local Optimization Procedure
				//for cur and his new neighbor
				this->DoLocalOptimization(cur, cur->Neighbor[1]);

				//for temp_1 and his new neighbor
				this->DoLocalOptimization(temp_1, temp_1->Neighbor[1]);

				//for temp_2 and his new neighbor
				this->DoLocalOptimization(temp_2, temp_2->Neighbor[1]);

				}

			//done, if dot locates too close, ignore it
			insert_not_done = false;
		}

		//on one side, search neighbors
		else if(det_2>= 0 && det_3>= 0)
			cur = cur->Neighbor[0];
		else if(det_1>= 0 && det_3>= 0)
			cur = cur->Neighbor[1];
		else if(det_1>= 0 && det_2>= 0)
			cur = cur->Neighbor[2];

		//cannot determine effectively 
		else if(det_1>0)
			cur = cur->Neighbor[1];
		else if(det_2>0)
			cur = cur->Neighbor[2];
		else if(det_3>0)
			cur = cur->Neighbor[0];
		else
			cur = cur->next;

		++index;
	}

	//reset the cur pointer
	this->cur = head;
}

void TriangleList::DoLocalOptimization(Triangle* tri_1, Triangle* tri_2)
{
	for(int i = 0; i<3; ++i)
	{
		if (tri_2->Vertex[i]!= tri_1->Vertex[0] &&
			tri_2->Vertex[i]!= tri_1->Vertex[1] &&
			tri_2->Vertex[i]!= tri_1->Vertex[2])
		{
			//initiate a matrix
			double matrix[] = {tri_2->Vertex[i]->projection.X-tri_1->Vertex[0]->projection.X,
							  tri_2->Vertex[i]->projection.Y-tri_1->Vertex[0]->projection.Y,
							  tri_2->Vertex[i]->projection.Z-tri_1->Vertex[0]->projection.Z,

							  tri_2->Vertex[i]->projection.X-tri_1->Vertex[1]->projection.X,
							  tri_2->Vertex[i]->projection.Y-tri_1->Vertex[1]->projection.Y,
							  tri_2->Vertex[i]->projection.Z-tri_1->Vertex[1]->projection.Z,

							  tri_2->Vertex[i]->projection.X-tri_1->Vertex[2]->projection.X,
							  tri_2->Vertex[i]->projection.Y-tri_1->Vertex[2]->projection.Y,
							  tri_2->Vertex[i]->projection.Z-tri_1->Vertex[2]->projection.Z
			};

			//get determinant
			if (this->GetDeterminant(matrix)>0)
			{
				//swap diagonal
				for (int j = 0; j<3; ++j)
				{
					for (int k = 0; k<3; ++k)
					{
						if (tri_1->Vertex[j]!= tri_2->Vertex[0] &&
							tri_1->Vertex[j]!= tri_2->Vertex[1] &&
							tri_1->Vertex[j]!= tri_2->Vertex[2] &&
							tri_2->Vertex[k]!= tri_1->Vertex[0] &&
							tri_2->Vertex[k]!= tri_1->Vertex[1] &&
							tri_2->Vertex[k]!= tri_1->Vertex[2])
						{
							//change vertices
							tri_1->Vertex[(j+2)%3] = tri_2->Vertex[k];
							tri_2->Vertex[(k+2)%3] = tri_1->Vertex[j];

							//change neighbors
							tri_1->Neighbor[(j+1)%3] = tri_2->Neighbor[(k+2)%3];
							tri_2->Neighbor[(k+1)%3] = tri_1->Neighbor[(j+2)%3];
							tri_1->Neighbor[(j+2)%3] = tri_2;
							tri_2->Neighbor[(k+2)%3] = tri_1;

							//pointer outside reoriented
							for (int l = 0; l<3; ++l)
							{
								if (tri_1->Neighbor[(j+1)%3]->Neighbor[l] == tri_2)
								{
									tri_1->Neighbor[(j+1)%3]->Neighbor[l] = tri_1;
									break;
								}
							}

							for (int l = 0; l<3; ++l)
							{
								if (tri_2->Neighbor[(k+1)%3]->Neighbor[l] == tri_1)
								{
									tri_2->Neighbor[(k+1)%3]->Neighbor[l] = tri_2;
									break;
								}
							}

							//check new neighbors
							this->DoLocalOptimization(tri_1, tri_1->Neighbor[j]);
							this->DoLocalOptimization(tri_1, tri_1->Neighbor[(j+1)%3]);
							this->DoLocalOptimization(tri_2, tri_2->Neighbor[k]);
							this->DoLocalOptimization(tri_2, tri_2->Neighbor[(k+1)%3]);

							//done
							return;
						}
					}
				}
			}

			//already optimized
			else break;
		}
	}	
}

void TriangleList::RemoveExtraTriangle()
{
	//extra triangle flag
	bool is_extra = false;
	//deletion pointer
	Triangle* del = head->next;

	while(del!= tail)
	{
		//check with 6 extra vertices
		for (int i = 0; i<6; ++i)
		{
			if (del->Vertex[0] == hex_vertices[i] ||
				del->Vertex[1] == hex_vertices[i] ||
				del->Vertex[2] == hex_vertices[i] ||
				IsInvisible(del))
			{
				is_extra = true;
				break;
			}
		}

		//is extra triangle
		if (is_extra)
		{
			cur->next = del->next;
			delete del;
			del = cur->next;
			size--;			
		}
		//not extra triangle
		else
		{
			cur = del;
			del = del->next;
		}

		//reset extra triangle flag
		is_extra = false;
	}

	//check the head
	for (int i = 0; i<6; ++i)
	{
		if (head->Vertex[0] == hex_vertices[i] ||
			head->Vertex[1] == hex_vertices[i] ||
			head->Vertex[2] == hex_vertices[i] ||
			IsInvisible(head))
		{
			is_extra = true;
			break;
		}
	}

	if (is_extra)
	{
		del = head;
		head = head->next;

		delete del;
		del = NULL;
		size--;
	}

	//reset cur pointer
	cur = head;
}

bool TriangleList::NotTooClose(Dot* dot_1, Dot* dot_2) const
{
	return (pow((dot_1->projection.X-dot_2->projection.X), 2)+
			pow((dot_1->projection.Y-dot_2->projection.Y), 2)+
			pow((dot_1->projection.Z-dot_2->projection.Z), 2)) > TOLERANCE;
}

bool TriangleList::MoveToNext()
{
	cur = cur->next;
	return (cur == tail)? false: true;
}

bool TriangleList::IsInvisible(Triangle* temp) const
{
	return 0;
}

double TriangleList::GetDeterminant(double matrix[]) const
{
	//inversed for left handed coordinate system
	return matrix[2]*matrix[4]*matrix[6]
		  +matrix[0]*matrix[5]*matrix[7]
		  +matrix[1]*matrix[3]*matrix[8]
		  -matrix[0]*matrix[4]*matrix[8]
		  -matrix[1]*matrix[5]*matrix[6]
		  -matrix[2]*matrix[3]*matrix[7];
}

int TriangleList::GetCurVerticesID(int index)
{
	return cur->Vertex[index]->id;
}
