#ifndef TRIANGLELIST_H
#define TRIANGLELIST_H

#include "Triangle.h"
#include "DotList.h"

class TriangleList
{
private:
	int size;
	Triangle* head;
	Triangle* tail;
	Triangle* cur;

	DotList* octahedron;
	Dot* hex_vertices[6];

public:
	TriangleList();
	~TriangleList();

	void DoTriangulation(DotList*);
	void InsertDot(Dot*);
	void DoLocalOptimization(Triangle*, Triangle*);
	void RemoveExtraTriangle();
	bool NotTooClose(Dot*, Dot*) const;
	bool MoveToNext();
	bool IsInvisible(Triangle*) const;
	double GetDeterminant(double[]) const;
	int GetCurVerticesID(int);
};

#endif