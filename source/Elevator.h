
#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "PacketStream.h"
#include "Constants.h"

class Elevator {

public:

	Elevator(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt1, int pExt2, b2World* pWorld);
	~Elevator();

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
	int xExtent1;
	int xExtent2;
	float yReset;
	float xReset;
	bool mActive;
	char* mUserData;
	int mSpeed;
	float mPauseTimer;
};

#endif