#ifndef SWITCH_H
#define SWITCH_H

#include "Iw2D.h"
#include "Door.h"
#include "IwSound.h"

class Switch {

public:

	Switch(const char* pImgName, CIwSVec2 pPos, Door* pDoor, b2World* pWorld);
	~Switch();

	bool isActive();
	void toggle();
	void draw();
	b2Vec2 getPosition();

private:

	CIw2DImage* mImage;
	CIwSVec2 mPos;
	bool mActive;
	Door* mDoor;
	int mImgOffset;
	b2Body* mBody;
	CIwSoundSpec* mToggleSoundSpec;
};

#endif