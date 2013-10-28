#ifndef PICKUP_H
#define PICKUP_H

#include "Iw2D.h"
#include "IwUI.h"
#include "Constants.h"
#include "Box2D\Box2D.h"
#include "tinyxml.h"
#include "PacketStream.h"
#include "s3eSound.h"
#include "s3e.h"
#include "IwSound.h"

class Pickup {

public:
	static int pickupCount;

	Pickup(const char* pImgName, CIwSVec2 pPos, char* pType, b2World* pWorld, bool pDynamic, bool pLoading);
	~Pickup();

	void update(); 
	void draw();
	int getStrength();
	int getID();
	void setID(int pID);
	b2Body* getBody();
	void pickUp();
	bool isPickedUp();
	void destroy(b2World* pWorld);
	void save(TiXmlDocument& pDoc);

	void serialize(PacketStream* );
	void deserialize(PacketStream* );

protected:
	b2Body* mBody;
	CIw2DImage* mImg;
	CIwSVec2 mPos;
	char* mType;
	bool PickedUp;
	float highBounce;
	float lowBounce;
	int mDir;
	int bounceSpeed;
	int mStrength;
	int mId;
	bool mDynamic;

	char     mVolumeString[256];
	s3eFile* FileHandle;
	int16*   SoundBuffer;
	int32    FileSize;
	int      Channel;

	CIwSoundSpec* mSoundSpec;
};

#endif