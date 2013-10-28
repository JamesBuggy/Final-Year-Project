#include "Checkpoint.h"

/**
* Constructor
* @param pPos the position of the checkpoint
* @param pSize the size of the checkpoint sensor
* @param pWorld the physics world the checkpoint is to be added to
*/
Checkpoint::Checkpoint(CIwSVec2 pPos, CIwSVec2 pSize, b2World* pWorld):
	mUserData("checkpoint"),
	mActivated(false) {

	float X = -(768/16) + (pSize.x*4) + (pPos.x*4) + 0.5;
	float Y = (1024/16) - (pSize.y*4) - (pPos.y*4) - 0.5;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(X, Y);
		
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.1f);
	mBody->SetFixedRotation(true);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox((pSize.x * (8/2)), (pSize.y * (8/2)));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 10.0f;
	fd.density = 10.0f;
	fd.restitution = 0.0f;
	fd.isSensor = true;
	fd.userData = mUserData;
	fd.filter.categoryBits = Constants::SENSOR;
	fd.filter.maskBits = Constants::PLAYER;

	mBody->CreateFixture(&fd);
	mBody->SetUserData(this);
}

/**
* Sets the checkpoints active state (set to inactive once the player hits it)
* @param pVal the desired state
*/
void Checkpoint::setActive(bool pVal) {

	mActivated = pVal;
}

/**
* gets the checkpoints active state
* @return the checkpoints state
*/
bool Checkpoint::isActive() {

	return mActivated;
}

/**
* Destructor
*/
Checkpoint::~Checkpoint() {



}