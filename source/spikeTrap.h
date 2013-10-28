#include "trap.h"
#include "Box2D/Box2D.h"
#include "Constants.h"

#ifndef SPIKETRAP_H
#define SPIKETRAP_H

class SpikeTrap : public Trap {

public:
	SpikeTrap(const char* pImgName, CIwSVec2 pPos, b2World* pWorld);
	~SpikeTrap();

	void update();

	bool isColliding(CIwSVec2 pPoint);
	void draw();

private:

	b2Body * mBody;
	

};

#endif