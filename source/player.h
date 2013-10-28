#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "IwUI.h"
#include "math.h"
#include <vector>
#include <string>
#include "PacketStream.h"
#include "Constants.h"
#include "tinyxml.h"
#include "s3eSound.h"
#include "s3e.h"
#include <iostream.h>
#include "IwSound.h"
#include <ctime>

#ifndef PLAYER_H
#define PLAYER_H

using namespace std;

class Player{

public:
	Player(const char* pImage, b2Vec2 pPos, b2World* pWorld, bool pLoading);
	~Player();

	void accelerate(b2Vec2 pForce);
	void update(float pDeltaTime);
	void draw();
	b2Body* getBody();

	void setDir(int pDir);
	CIw2DImage* getImage();

	CIwSVec2 getPos();

	int getMeleeStr();
	int getHealth();
	void jump();
	void wallGrab();
	void kick();
	void punch();
	void idleLegs();
	void idleTorso();
	void goIdle();
	void kill();
	void damageBoost(int pVal);
	void speedBoost(int pVal);
	void heal(int pVal);
	void setCanRun(bool pVal);
	void applyDamage(int pDamage);
	void onPlatform(bool pVal);
	void finish();
	bool isFinished();

	CIwSVec2 mPos;
	bool loadCheckpoint;

	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);

	void save(TiXmlDocument& pDoc);

	int jumped;
private:

	b2Body* mBody;
	b2Fixture* mFixture;
	CIw2DImage* mImage;

	//CIwSVec2 mPos;

	bool mFinished;

	int direction;
	int prevDir;
	bool punching;
	bool kicking;
	bool canRun;

	float frameHeight;
	float frameWidth;

	// Torso frame info
	int curFrameTorso;
	int numFramesTorso;
	int frameYTorso;
	int frameXTorso;

	// Leg frame info
	int curFrameLegs;
	int numFramesLegs;
	int frameYLegs;
	int frameXLegs;

	int frameDelayTorso;
	int frameDelayLegs;
	int maxFrameDelay;
	float scale;

	bool grabbedWall;
	int wallJumpDir;

	int collWait;
	char* mType;

	int mMeleeStr;
	int mBaseMeleeStr;
	int mSpeedMult;
	int mHealth;
	bool mOnPlatform;

	float mSpeedBoostCounter;
	float mDmgBoostCounter;
	CIw2DImage* mSpeedIcon;
	CIw2DImage* mDamageIcon;

	// Audio stuff
	char     mVolumeString[256];
	s3eFile* kickFileHandle;
	int16*   kickSoundBuffer;
	int32    kickFileSize;
	int      kickChannel;

	s3eFile* punchFileHandle;
	int16*   punchSoundBuffer;
	int32    punchFileSize;
	int      punchChannel;

	//CIwSoundSpec* kickSoundSpec;
	//CIwSoundSpec* punchSoundSpec;

	CIwSoundSpec* mSpawnSoundSpec;
	
	vector<CIwSoundSpec*> mKickSounds;
	vector<CIwSoundSpec*> mPunchSounds;
};

#endif