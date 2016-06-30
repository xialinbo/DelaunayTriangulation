/*=========================================================================
This file is the header file for spherical delaunay 3D triangulation.
Copyright (c) 2010 by Xia Linbo. All right reserved.
Version: 26/07/2010
=========================================================================*/
#include <iostream>
#include <cmath>

//3D vector
class Vector3D
{
public:
	//coordinate
	double X, Y, Z;

	//color
	int R, G, B;

	//constructors
	Vector3D();
	Vector3D(double, double, double, int, int, int);
};

//linked list containing dot cloud information
class DotList
{
	//can be accessed by the triangle list
	friend class MeshList;

private:
	struct ListNode
	{
		//coordinate in Cartesian system(x, y, z)
		Vector3D coordinate;
		
		//projected coordinate(x, y, z)
		Vector3D projection;

		//depth
		double r;

		//ID
		int id;

		//linking pointer
		ListNode* next;
	};

	//external elements
	int size;
	ListNode* head;
	ListNode* tail;
	ListNode* cur;

public:
	//constructor
	DotList();

	//destructor
	~DotList();

	//insert a dot
	void addDot(Vector3D);

	//advance the cur pointer and check availability
	bool IsNextValid();

	//get current projection
	Vector3D getCurProjection();

	//get current coordinate
	Vector3D getCurCoordinate();

	//get size
	int getSize();
};

//list containing arranged triangle mesh
class MeshList
{
private:
	struct ListNode{
		//3 pointer pointing to 3 vertices
		DotList::ListNode* P[3];

		//3 pointer pointing to 3 neighbors
		ListNode* N[3];

		//1 pointer pointing to the next list node
		ListNode* next;
	};

	//external elements
	int size;
	ListNode* head;
	ListNode* tail;
	ListNode* cur;

	//the octahedron vertices pointer
	DotList* octahedron;
	DotList::ListNode* hex_vertices[6];

public:
	//constructor
	MeshList();

	//destructor
	~MeshList();

	//Delaunay triangulation on a unit sphere
	void doTriangulation(DotList*);

	//insert a dot(The Algorithm)
	void insertDot(DotList::ListNode*);

	//local optimization procedure
	void doLocalOptimization(ListNode*, ListNode*);

	//remove extral mesh
	void removeExtraMesh();

	//check whether dots are too close to each other
	bool NotTooClose(DotList::ListNode*, DotList::ListNode*) const;

	//advance the cur pointer and check availability
	bool IsNextValid();

	//check whether a triangle is steep
	bool IsInvisible(ListNode*) const;

	//get determinant
	double getDet(double[]) const;
	
	//get current triangle vertices IDs
	int getCurVerticesID(int);

	//get size
	int getSize();

};