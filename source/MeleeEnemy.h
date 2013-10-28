#include "Enemy.h"

#ifndef MELEE_ENEMY_H
#define MELEE_ENEMY_H

class MeleeEnemy : public Enemy {

public:

	MeleeEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, bool pLoading, int pDiff, int pPatrolA, int pPatrolB);
	~MeleeEnemy();

	void update(bool pHost);
	void setAttack();
	void setIdle();
	void draw();
	void punch();
	
private:

	int mFrame;
	int mVerFrame;
	int mFrameWidth;
	int mFrameCount;
	int mFrameDelay;
	int mAttackRange;
	bool punching;
	float mPunchTimer;
}; 

#endif