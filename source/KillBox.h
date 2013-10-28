
#ifndef KILLBOX_H
#define KILLBOX_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "Constants.h"

class KillBox {

public:
	
	KillBox(b2Vec2 pPos, b2Vec2 pSize, b2World* pWorld);
	~KillBox();

private:

	b2Body* mBody;
	char* mUserData;
};

#endif