#ifndef DOOR_H
#define DOOR_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "PacketStream.h"
#include "Constants.h"

class Door {

public:

	Door(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt, int pDir, b2World* pWorld);
	~Door();

	void update();
	void draw();
	void toggle();

	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);

private:

	b2Body* mBody;
	CIw2DImage* mImage;
	float mWidth;
	int mDirection;
	int xExtent;
	bool mActive;
	char* mUserData;
	int mSpeed;
};

#endif