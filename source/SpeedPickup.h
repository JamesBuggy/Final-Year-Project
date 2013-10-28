
#ifndef SPEEDPICKUP_H
#define SPEEDPICKUP_H

#include "Pickup.h"

class SpeedPickup : public Pickup{

public:

	SpeedPickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading);
	~SpeedPickup();

private:


};

#endif