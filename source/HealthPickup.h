
#ifndef HEALTHPICKUP_H
#define HEALTHPICKUP_H

#include "Pickup.h"

class HealthPickup : public Pickup{

public:

	HealthPickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading);
	~HealthPickup();
private:


};

#endif