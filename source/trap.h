#include "Iw2D.h"
#include "IwUI.h"
#include <string>
#include "Constants.h"

#ifndef TRAP_H
#define TRAP_H

using namespace std;

class Trap {

public:

	Trap(const char* pImgName, CIwSVec2 pPos);
	~Trap();

	virtual void update() = 0; 
	virtual void draw() = 0;

	virtual bool isColliding(CIwSVec2 pPoint) = 0;

protected:

	
	CIwSVec2 mPos;
	CIw2DImage* mImg;
	char* mType;
};

#endif