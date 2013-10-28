#include "trap.h"
#include "math.h"
#include "Box2D/Box2D.h"

#ifndef BLADETRAP_H
#define BLADETRAP_H

class BladeTrap : public Trap {

public:

	BladeTrap(const char* pImgName, CIwSVec2 pPos, b2World* pWorld);
	~BladeTrap();

	void update();
	bool isColliding(CIwSVec2 pPoint);
	void draw();

private:

	
	b2Body* mBody;
	int mFrame;
	int mFrames;

};

#endif