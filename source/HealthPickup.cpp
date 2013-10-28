#include "HealthPickup.h"

/**
* Constructor
* @param pPos the position of the pickup
* @param pWorld the physics world to add the pickup to
* @param pDynamic does the pickup fall under gravity
* @param pLoading is the pickup loaded from a file
*/
HealthPickup::HealthPickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading):
Pickup("Images/greenOrb.png", pPos, "healthPickup", pWorld, pDynamic, pLoading) {

}

/**
* Destructor
*/
HealthPickup::~HealthPickup() {

}