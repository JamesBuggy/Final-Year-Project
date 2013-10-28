
#ifndef LEVELEND_H
#define LEVELEND_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "Constants.h"

class LevelEnd {

public:
	
	LevelEnd(b2Vec2 pPos, b2Vec2 pSize, const char* pImageName, b2World* pWorld);
	~LevelEnd();

	void draw();
	void setActive();
	bool isActive();

private:

	b2Body* mBody;
	CIw2DImage* mImage;
	char* mUserData;
	bool mActive;
};

#endif