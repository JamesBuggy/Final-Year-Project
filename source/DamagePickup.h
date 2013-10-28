
#ifndef DAMAGEPICKUP_H
#define DAMAGEPICKUP_H

#include "Pickup.h"

class DamagePickup : public Pickup{

public:

	DamagePickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading);
	~DamagePickup();

private:


};

#endif