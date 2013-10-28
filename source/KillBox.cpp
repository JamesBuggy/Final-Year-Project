
#include "KillBox.h"

/**
* Constructor
* @param pPos the position of the killbox
* @param pSize the size of the killbox
* @param pWorld the physics world to add the pickup to
*/
KillBox::KillBox(b2Vec2 pPos, b2Vec2 pSize, b2World* pWorld):
	mUserData("killBox") {

	float X = (((-(768/64)*4)+(0.5*4))+(pPos.x*4)) + 0.5;
	float Y = ((((1024/64)*4)-(0.5*4)) + (-pPos.y*4)) - 0.5;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(X, Y);
	mBody = pWorld->CreateBody(&bodyDef);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox((pSize.x * (8/2)), (pSize.y * (8/2)));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.isSensor = true;
	fd.filter.categoryBits = Constants::SENSOR;
	fd.filter.maskBits = Constants::PLAYER | Constants::PICKUP;
	fd.userData = mUserData;

	mBody->CreateFixture(&fd);
	mBody->SetUserData(this);
}