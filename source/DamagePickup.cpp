#include "DamagePickup.h"

/**
* Constructor
* @param pPos the position of the pickup
* @param pWorld the physics world to add the pickup to
* @param pDynamic does the pickup fall under gravity
* @param pLoading is the pickup loaded from a file
*/
DamagePickup::DamagePickup(CIwSVec2 pPos, b2World* pWorld, bool pDynamic, bool pLoading):
Pickup("Images/redOrb.png", pPos, "damagePickup", pWorld, pDynamic, pLoading) {

}

/**
* Destructor
*/
DamagePickup::~DamagePickup() {

}