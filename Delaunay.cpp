/*=========================================================================
This file is the header file for spherical delaunay 3D triangulation.
Copyright (c) 2010 by Xia Linbo. All right reserved.
Version: 26/07/2010
=========================================================================*/
#include "Delaunay.h"

#define RADIUS 100
#define MIN 0.000000000001
#define MAX 50000

/*************************************************************************************************/
//3D vector constructor
Vector3D::Vector3D()
{
	X= 0;
	Y= 0;
	Z= 0;

	R= 255;
	G= 248;
	B= 220;
}

Vector3D::Vector3D(double x, double y, double z, int r, int g, int b)
{
	X= x;
	Y= y;
	Z= z;

	R= r;
	G= g;
	B= b;
}

/*************************************************************************************************/
//linked list containing dot cloud information
//constructor
DotList::DotList()
{
	size= 0;
	head= new ListNode;
	head->next= NULL;
	tail= head;
	cur= head;
}

//destructor
DotList::~DotList()
{
	while (head!= tail)
	{
		cur= head->next;
		delete head;
		head= cur;

		size--;
	}

	delete head;
	head= NULL;
	tail= NULL;
	cur= NULL;
}

//insert a dot
void DotList::addDot(Vector3D point)
{
	//assign values
	tail->coordinate= point;

	//get r
	tail->r= sqrt(pow(tail->coordinate.X, 2)
		+pow(tail->coordinate.Y, 2)
		+pow(tail->coordinate.Z, 2));

	//get normalized x
	tail->projection.X= RADIUS*(tail->coordinate.X)/tail->r;

	//get normalized y
	tail->projection.Y= RADIUS*(tail->coordinate.Y)/tail->r;

	//get normalized z
	tail->projection.Z= RADIUS*(tail->coordinate.Z)/tail->r;

	//set ID
	tail->id= size;

	//add next empty list node
	tail->next= new ListNode;
	tail= tail->next;
	tail->next= NULL;

	++size;
}

//advance the cur pointer and check availability
bool DotList::IsNextValid()
{
	cur= cur->next;
	return (cur==tail)? false: true;
}

//get current projection
Vector3D DotList::getCurProjection()
{
	return cur->projection;
}

//get current coordinate
Vector3D DotList::getCurCoordinate()
{
	return cur->coordinate;
}

//get size
int DotList::getSize()
{
	return size;
}

/*************************************************************************************************/
//list containing arranged triangle mesh
//constructor
MeshList::MeshList()
{
	//initializing
	size= 0;
	head= new ListNode;

	for (int i=0; i<3; ++i)
	{
		head->P[i]= NULL;
		head->N[i]= NULL;
	}

	head->next= NULL;

	tail= head;
	cur= head;

	//create vertices and faces of the octahedron
	octahedron= new DotList();
	ListNode* octa_triangles[8];

	//insert initial vertices
/*	octahedron->addDot(Vector3D(RADIUS*0.8, 0, RADIUS*0.6, 105, 105, 255));
	octahedron->addDot(Vector3D(RADIUS*0.8, 0, -RADIUS*0.6, 105, 105, 255));
	octahedron->addDot(Vector3D(-RADIUS*0.8, 0, RADIUS*0.6, 105, 105, 255));
	octahedron->addDot(Vector3D(-RADIUS*0.8, 0, -RADIUS*0.6, 105, 105, 255));
	octahedron->addDot(Vector3D(0, RADIUS, 0, 105, 105, 255));
	octahedron->addDot(Vector3D(0, -RADIUS, 0, 105, 105, 255));
*/
	octahedron->addDot(Vector3D(RADIUS, 0, 0, 105, 105, 255));
	octahedron->addDot(Vector3D(0, 0, RADIUS, 105, 105, 255));
	octahedron->addDot(Vector3D(-RADIUS, 0, 0, 105, 105, 255));
	octahedron->addDot(Vector3D(0, 0, -RADIUS, 105, 105, 255));
	octahedron->addDot(Vector3D(0, RADIUS, 0, 105, 105, 255));
	octahedron->addDot(Vector3D(0, -RADIUS, 0, 105, 105, 255));

	for(int i=0; i<6; ++i)
	{
		//coding
		hex_vertices[i]= octahedron->cur;

		//move forward, octahedron is a friend class of "this"
		octahedron->cur= octahedron->cur->next;
	}

	//create 8 faces
	for(int i=0; i<4; ++i)
	{
		tail->P[0]= hex_vertices[4];
		tail->P[1]= hex_vertices[i];
		tail->P[2]= hex_vertices[(i+1)%4];

		//set individual face pointer
		octa_triangles[i]= tail;

		//create the next
		tail->next= new ListNode;
		tail= tail->next;

		for (int i=0; i<3; ++i)
		{
			tail->P[i]= NULL;
			tail->N[i]= NULL;
		}

		tail->next= NULL;

		++size;
	}

	for(int i=4; i<8; ++i)
	{
		tail->P[0]= hex_vertices[5];
		tail->P[1]= hex_vertices[(i-3)%4];
		tail->P[2]= hex_vertices[(i-4)];

		//set individual face pointer
		octa_triangles[i]= tail;

		//create the next
		tail->next= new ListNode;
		tail= tail->next;

		for (int i=0; i<3; ++i)
		{
			tail->P[i]= NULL;
			tail->N[i]= NULL;
		}

		tail->next= NULL;

		++size;
	}

	//set pointers pointing to neighbors
	for(int i=0; i<4; ++i)
	{
		octa_triangles[i]->N[0]= octa_triangles[(i+3)%4];
		octa_triangles[i]->N[1]= octa_triangles[i+4];
		octa_triangles[i]->N[2]= octa_triangles[(i+1)%4];
	}

	for(int i=0; i<4; ++i)
	{
		octa_triangles[i+4]->N[0]= octa_triangles[(i+1)%4+4];
		octa_triangles[i+4]->N[1]= octa_triangles[i];
		octa_triangles[i+4]->N[2]= octa_triangles[(i+3)%4+4];
	}
}

//destructor
MeshList::~MeshList()
{
	while(head!= tail)
	{
		cur= head->next;
		delete head;
		head= cur;

		size--;
	}
		
	delete head;
	head= NULL;
	tail= NULL;
	cur= NULL;

	delete octahedron;
	octahedron= NULL;
	for(int i=0; i<6; ++i)
		hex_vertices[i]= NULL;
}

/*************************************************************************************************/
//Delaunay triangulation on a unit sphere
void MeshList::doTriangulation(DotList* dots)
{
	bool next_valid= (dots->getSize())? true:false;

	while(next_valid)
	{
		//insert a dot into the triangular mesh
		this->insertDot(dots->cur);

		//move to the next triangle
		next_valid= dots->IsNextValid();
	}

	//reset cur pointer for dot list
	dots->cur= dots->head;
}

//insert a dot(The Algorithm)
void MeshList::insertDot(DotList::ListNode* dot)
{
	//coordinate parameters
	double matrix[]= {0, 0, 0,
					  0, 0, 0,
					  0, 0, 0
	};

	//determinants
	double det_1= 0, det_2= 0, det_3= 0;
	//stop flag
	bool insert_not_done= true;
	//max index
	int index= 0;

	while(insert_not_done && index<MAX)
	{
		//assign coordinate value
		matrix[0]= cur->P[0]->projection.X;
		matrix[1]= cur->P[0]->projection.Y;
		matrix[2]= cur->P[0]->projection.Z;
		matrix[3]= cur->P[1]->projection.X;
		matrix[4]= cur->P[1]->projection.Y;
		matrix[5]= cur->P[1]->projection.Z;
		matrix[6]= dot->projection.X;
		matrix[7]= dot->projection.Y;
		matrix[8]= dot->projection.Z;

		//get the first determinant
		det_1= this->getDet(matrix);

		//assign coordinate value
		matrix[0]= cur->P[1]->projection.X;
		matrix[1]= cur->P[1]->projection.Y;
		matrix[2]= cur->P[1]->projection.Z;

		matrix[3]= cur->P[2]->projection.X;
		matrix[4]= cur->P[2]->projection.Y;
		matrix[5]= cur->P[2]->projection.Z;

		//get the second determinant
		det_2= this->getDet(matrix);

		//assign coordinate value
		matrix[0]= cur->P[2]->projection.X;
		matrix[1]= cur->P[2]->projection.Y;
		matrix[2]= cur->P[2]->projection.Z;

		matrix[3]= cur->P[0]->projection.X;
		matrix[4]= cur->P[0]->projection.Y;
		matrix[5]= cur->P[0]->projection.Z;

		//get the third determinant
		det_3= this->getDet(matrix);

		//determinant conditions
		//inside the triangle, create new triangular mesh
		if (det_1>=0 && det_2>=0 && det_3>=0)
		{
			//if not too close
			if (this->NotTooClose(dot, cur->P[0])&&
				this->NotTooClose(dot, cur->P[1])&&
				this->NotTooClose(dot, cur->P[2]))
			{
				//create 2 new list node
				ListNode* temp_1= new ListNode;
				ListNode* temp_2= new ListNode;

				//assign vertex pointer
				temp_1->P[0]= dot;
				temp_1->P[1]= cur->P[1];
				temp_1->P[2]= cur->P[2];

				temp_2->P[0]= dot;
				temp_2->P[1]= cur->P[2];
				temp_2->P[2]= cur->P[0];

				cur->P[0]= dot;
				cur->P[1]= temp_2->P[2];
				cur->P[2]= temp_1->P[1];

				//change neighbor pointer
				temp_1->N[0]= cur;
				temp_1->N[1]= cur->N[1];
				temp_1->N[2]= temp_2;

				temp_2->N[0]= temp_1;
				temp_2->N[1]= cur->N[2];
				temp_2->N[2]= cur;

				cur->N[1]= cur->N[0];
				cur->N[0]= temp_2;
				cur->N[2]= temp_1;

				//pointer outside reoriented
				//temp_1
				for (int i=0; i<3; ++i)
				{
					if (temp_1->N[1]->N[i]==cur)
					{
						temp_1->N[1]->N[i]= temp_1;
						break;
					}
				}
			
				//temp_2
				for (int i=0; i<3; ++i)
				{
					if (temp_2->N[1]->N[i]==cur)
					{
						temp_2->N[1]->N[i]= temp_2;
						break;
					}
				}

				//insert and link new list node
				temp_2->next= cur->next;
				temp_1->next= temp_2;
				cur->next= temp_1;

				size+= 2;

				//Local Optimization Procedure
				//for cur and his new neighbor
				this->doLocalOptimization(cur, cur->N[1]);

				//for temp_1 and his new neighbor
				this->doLocalOptimization(temp_1, temp_1->N[1]);

				//for temp_2 and his new neighbor
				this->doLocalOptimization(temp_2, temp_2->N[1]);

				}

			//done, if dot locates too close, ignore it
			insert_not_done= false;
		}

		//on one side, search neighbors
		else if(det_2>=0 && det_3>=0)
			cur= cur->N[0];
		else if(det_1>=0 && det_3>=0)
			cur= cur->N[1];
		else if(det_1>=0 && det_2>=0)
			cur= cur->N[2];

		//cannot determine effectively 
		else if(det_1>0)
			cur= cur->N[1];
		else if(det_2>0)
			cur= cur->N[2];
		else if(det_3>0)
			cur= cur->N[0];
		else
			cur= cur->next;

		++index;
	}

	//reset the cur pointer
	this->cur= head;
}

//local optimization procedure
void MeshList::doLocalOptimization(ListNode* tri_1, ListNode* tri_2)
{
	for(int i=0; i<3; ++i)
	{
		if (tri_2->P[i]!=tri_1->P[0] &&
			tri_2->P[i]!=tri_1->P[1] &&
			tri_2->P[i]!=tri_1->P[2])
		{
			//initiate a matrix
			double matrix[]= {tri_2->P[i]->projection.X-tri_1->P[0]->projection.X,
							  tri_2->P[i]->projection.Y-tri_1->P[0]->projection.Y,
							  tri_2->P[i]->projection.Z-tri_1->P[0]->projection.Z,

							  tri_2->P[i]->projection.X-tri_1->P[1]->projection.X,
							  tri_2->P[i]->projection.Y-tri_1->P[1]->projection.Y,
							  tri_2->P[i]->projection.Z-tri_1->P[1]->projection.Z,

							  tri_2->P[i]->projection.X-tri_1->P[2]->projection.X,
							  tri_2->P[i]->projection.Y-tri_1->P[2]->projection.Y,
							  tri_2->P[i]->projection.Z-tri_1->P[2]->projection.Z
			};

			//get determinant
			if (this->getDet(matrix)>0)
			{
				//swap diagonal
				for (int j=0; j<3; ++j)
				{
					for (int k=0; k<3; ++k)
					{
						if (tri_1->P[j]!= tri_2->P[0] &&
							tri_1->P[j]!= tri_2->P[1] &&
							tri_1->P[j]!= tri_2->P[2] &&
							tri_2->P[k]!= tri_1->P[0] &&
							tri_2->P[k]!= tri_1->P[1] &&
							tri_2->P[k]!= tri_1->P[2])
						{
							//change vertices
							tri_1->P[(j+2)%3]= tri_2->P[k];
							tri_2->P[(k+2)%3]= tri_1->P[j];

							//change neighbors
							tri_1->N[(j+1)%3]= tri_2->N[(k+2)%3];
							tri_2->N[(k+1)%3]= tri_1->N[(j+2)%3];
							tri_1->N[(j+2)%3]= tri_2;
							tri_2->N[(k+2)%3]= tri_1;

							//pointer outside reoriented
							for (int l=0; l<3; ++l)
							{
								if (tri_1->N[(j+1)%3]->N[l]==tri_2)
								{
									tri_1->N[(j+1)%3]->N[l]= tri_1;
									break;
								}
							}

							for (int l=0; l<3; ++l)
							{
								if (tri_2->N[(k+1)%3]->N[l]==tri_1)
								{
									tri_2->N[(k+1)%3]->N[l]= tri_2;
									break;
								}
							}

							//check new neighbors
							this->doLocalOptimization(tri_1, tri_1->N[j]);
							this->doLocalOptimization(tri_1, tri_1->N[(j+1)%3]);
							this->doLocalOptimization(tri_2, tri_2->N[k]);
							this->doLocalOptimization(tri_2, tri_2->N[(k+1)%3]);

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

/*************************************************************************************************/
//remove extral mesh
void MeshList::removeExtraMesh()
{
	//extra triangle flag
	bool is_extra= false;
	//deletion pointer
	ListNode* del= head->next;

	while(del!= tail)
	{
		//check with 6 extra vertices
		for (int i=0; i<6; ++i)
		{
			if (del->P[0]==hex_vertices[i] ||
				del->P[1]==hex_vertices[i] ||
				del->P[2]==hex_vertices[i] ||
				IsInvisible(del))
			{
				is_extra= true;
				break;
			}
		}

		//is extra triangle
		if (is_extra)
		{
			cur->next= del->next;
			delete del;
			del= cur->next;
			size--;			
		}
		//not extra triangle
		else
		{
			cur= del;
			del= del->next;
		}

		//reset extra triangle flag
		is_extra= false;
	}

	//check the head
	for (int i=0; i<6; ++i)
	{
		if (head->P[0]==hex_vertices[i] ||
			head->P[1]==hex_vertices[i] ||
			head->P[2]==hex_vertices[i] ||
			IsInvisible(head))
		{
			is_extra= true;
			break;
		}
	}

	if (is_extra)
	{
		del= head;
		head= head->next;

		delete del;
		del= NULL;
		size--;
	}

	//reset cur pointer
	cur= head;
}

//check whether dots are too close to each other
bool MeshList::NotTooClose(DotList::ListNode* dot_1, DotList::ListNode* dot_2) const
{
	return (pow((dot_1->projection.X-dot_2->projection.X), 2)+
			pow((dot_1->projection.Y-dot_2->projection.Y), 2)+
			pow((dot_1->projection.Z-dot_2->projection.Z), 2))>MIN;
}

//advance the cur pointer and check availability
bool MeshList::IsNextValid()
{
	cur= cur->next;
	return (cur==tail)? false: true;
}
/*
//check whether a triangle is steep
bool MeshList::IsInvisible(ListNode* temp) const
{
	return (abs(temp->P[0]->r-temp->P[1]->r)
		   +abs(temp->P[1]->r-temp->P[2]->r)
		   +abs(temp->P[2]->r-temp->P[0]->r))/
		   (temp->P[0]->r+temp->P[1]->r+temp->P[2]->r)> 0.15
		   ||temp->P[0]->r> 500;
}

bool IsInvisible(ListNode* temp) const
{
	//area ratio of triangle projection
	double area;

	//initial triangle
	double matrix[]= {1, 1, 1,
					  temp->P[1]->coordinate.X- temp->P[0]->coordinate.X,
					  temp->P[1]->coordinate.Y- temp->P[0]->coordinate.Y,
					  temp->P[1]->coordinate.Z- temp->P[0]->coordinate.Z,
					  temp->P[2]->coordinate.X- temp->P[0]->coordinate.X,
					  temp->P[2]->coordinate.Y- temp->P[0]->coordinate.Y,
					  temp->P[2]->coordinate.Z- temp->P[0]->coordinate.Z
	};
	area= fabs(this->getDet(matrix))*3/pow(temp->P[0]->r+temp->P[1]->r+temp->P[2]->r, 2);

	//projected triangle
	matrix[3]= temp->P[1]->projection.X- temp->P[0]->projection.X;
	matrix[4]= temp->P[1]->projection.Y- temp->P[0]->projection.Y;
	matrix[5]= temp->P[1]->projection.Z- temp->P[0]->projection.Z;
	matrix[6]= temp->P[2]->projection.X- temp->P[0]->projection.X;
	matrix[7]= temp->P[2]->projection.Y- temp->P[0]->projection.Y;
	matrix[8]= temp->P[2]->projection.Z- temp->P[0]->projection.Z;
		
	return area> 3*fabs(this->getDet(matrix))/(RADIUS*RADIUS);
}
*/
bool MeshList::IsInvisible(ListNode* temp) const
{
	return 0;
}

//get determinant
double MeshList::getDet(double matrix[]) const
{
	//inversed for left handed coordinate system
	return matrix[2]*matrix[4]*matrix[6]
		  +matrix[0]*matrix[5]*matrix[7]
		  +matrix[1]*matrix[3]*matrix[8]
		  -matrix[0]*matrix[4]*matrix[8]
		  -matrix[1]*matrix[5]*matrix[6]
		  -matrix[2]*matrix[3]*matrix[7];
}

//get current triangle vertices IDs
int MeshList::getCurVerticesID(int index)
{
	return cur->P[index]->id;
}

//get size
int MeshList::getSize()
{
	return size;
}