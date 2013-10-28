
#ifndef PLATFORM_H
#define PLATFORM_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "PacketStream.h"
#include "Constants.h"

class Platform {

public:

	Platform(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt1, int pExt2, b2World* pWorld);
	~Platform();

	void update();
	void draw();

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
	int mSpeed;
	char* mType;
};


#endif