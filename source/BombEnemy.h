#include "Enemy.h"

#ifndef BOMB_ENEMY_H
#define BOMB_ENEMY_H

class BombEnemy : public Enemy {

public:

	BombEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, bool pLoading, int pDiff, int pPatrolA, int pPatrolB);
	~BombEnemy();

	void update();
	
	void draw();
	void arm();

private:

	int mFrameHeight;
	int mFrameWidth;
	int mNumFrames;
	int mFrameY;
	int mFrameX;
	int mFrame;
	int mFrameDelay;

	b2Fixture* detSensor;
	int mSensorTtl;
	bool mDetonated;
	bool mArmed;
	int mTtl;

	void detonate();

	char* mDetSensorData;

}; 

#endif