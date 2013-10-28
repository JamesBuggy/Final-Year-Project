
#ifndef CRATESPAWNER_H
#define CRATESPAWNER_H

#include "Iw2D.h"
#include "Box2D\Box2D.h"
#include "Constants.h"

class CrateSpawner {

public:

	CrateSpawner(char* pType, b2Vec2 pSpawnerPos, b2Vec2 pSwitchPos, b2World* pWorld);
	~CrateSpawner();

	b2Body* getSpawner();
	b2Body* getSwitch();
	char* getType();
	void toggleSwitch();
	void draw();

private:

	b2Body* mSpawnerBody;
	b2Body* mSwitchBody;
	CIw2DImage* mSpawnerImg;
	CIw2DImage* mSwitchImg;
	int switchImgOffset;
	char* mType;
};

#endif