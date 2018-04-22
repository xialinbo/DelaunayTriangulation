#ifndef DOTLIST_H
#define DOTLIST_H

#include "Dot.h"

class DotList
{
private:
	int size;
	Dot* head;
	Dot* tail;
	Dot* cur;

public:
	DotList();
	~DotList();
	void AddDot(Vector3D);
	bool MoveToNext();
	void ResetCur();
	Dot* GetCurDot();
};

#endif