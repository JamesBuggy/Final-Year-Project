
#ifndef ACTIVEPLATFORM_H
#define ACTIVEPLATFORM_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "PacketStream.h"
#include "Constants.h"
#include "IwSound.h"

class ActivatablePlatform {

public:

	ActivatablePlatform(const char* pImage, CIwSVec2 pPos, CIwSVec2 pSwitchPos, float pWidth, int pExt1, int pExt2, b2World* pWorld);
	~ActivatablePlatform();

	void update();
	void draw();
	void toggle();
	b2Body* getSwitch();

	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);

private:

	b2Body* mBody;
	b2Body* mSwitchBody;
	CIw2DImage* mImage;
	CIw2DImage* mSwitchImg;
	float mWidth;
	int mDirection;
	int xExtent1;
	int xExtent2;
	float yReset;
	int mSpeed;
	char* mType;
	bool mActive;
	int switchImgOffset;

	CIwSoundSpec* mToggleSoundSpec;
};


#endif