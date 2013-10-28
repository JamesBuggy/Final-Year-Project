#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "IwUI.h"
#include "PacketStream.h"
#include "Constants.h"
#include "tinyxml.h"
#include <math.h>
#include "player.h"
#include "IwSound.h"

#ifndef ENEMY_H
#define ENEMY_H

class Enemy {

public:

	Enemy(const char * pImgName, CIwSVec2 pPos, int pID, int pDiff, int pPatrolA, int pPatrolB);
	~Enemy();

	b2Body* getBody();
	void ApplyDamage(int pDamage);
	void attack();
	void idle();

	virtual void update(bool pHost);
	bool isAlive();
	virtual void draw() = 0;

	void setTarget(b2Vec2 pTarget);
	int getID();
	bool isAttacking();
	void kill();
	void stun();

	void setPlayer(Player* pPlayer);
	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);
	void save(TiXmlDocument& pDoc);

protected:

	b2Body* mBody;
	int mID;
	char* mType;
	char* mSensorData;
	CIwSVec2 mPos;
	CIw2DImage* mImg;
	float mHealth;
	float mMaxHealth;
	int mStrength;
	bool mAlive;
	float mSpriteScale;
	b2Vec2 mPrevPosition;
	b2Vec2 mTarget;
	int mDirection;
	int mDrawDirection;
	
	bool mCanMove;
	float mStunTimer;

	int mPatrolPointA;
	int mPatrolPointB;

	int mPatSave1;
	int mPatSave2;

	Player* mPlayer;

	enum mStateCat {
	
		ATTACK = 0,
		IDLE = 1
	};
	mStateCat mState;


	CIwSoundSpec* mDeathSoundSpec;
};

#endif