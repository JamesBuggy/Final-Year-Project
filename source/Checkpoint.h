
#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "Constants.h"

class Checkpoint {

public:

	Checkpoint(CIwSVec2 pPos, CIwSVec2 pSize, b2World* pWorld);
	~Checkpoint();

	void setActive(bool pVal);
	bool isActive();

private:

	b2Body* mBody;
	char* mUserData;
	bool mActivated;
};

#endif