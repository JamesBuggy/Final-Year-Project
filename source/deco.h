#include "Iw2D.h"
#include "IwUI.h"

#ifndef DECO_H
#define DECO_H

class Deco {

public:
	Deco(const char* pImage, const char* pRes, CIwSVec2 pPos, CIwSVec2 pSize);
	~Deco();

	void draw();

private:

	CIw2DImage* mImage;
	CIwSVec2 mPos;
	CIwSVec2 mSize;

};

#endif