#include "SpeedPickup.h"

/**
* Constructor
* @param pPos the position of the pickup
* @param pWorld the physics world to add the pickup to
* @param pDynamic does the pickup fall under gravity
* @param pLoading is the pickup loaded from a file
*/
SpeedPickup::SpeedPickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading):
Pickup("Images/blueOrb.png", pPos, "speedPickup", pWorld, pDynamic, pLoading) {
	printf("creating pickup");
}

/**
* Destructor
*/
SpeedPickup::~SpeedPickup() {

}