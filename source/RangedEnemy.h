#include "Enemy.h"

#ifndef RANGED_ENEMY_H
#define RANGED_ENEMY_H

class RangedEnemy : public Enemy {

public:

	RangedEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, int pDiff, int pPatrolA, int pPatrolB);
	~RangedEnemy();

	void update();

private:


}; 

#endif