#include "RangedEnemy.h"

RangedEnemy::RangedEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, int pDiff, int pPatrolA, int pPatrolB):
	Enemy("rangedEnemy.png", pPos, pID, pDiff, pPatrolA, pPatrolB) {

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0, 0);
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.1f);
	mBody->SetFixedRotation(true);

	mType = "enemy";
	mBody->SetUserData(this);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox((0.4f * (8/2))*1, (0.7f * (8/2))*1);

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 0.4f;
	fd.density = 10.0f;
	fd.restitution = 0.0f;

	mBody->CreateFixture(&fd);

	b2PolygonShape sensorShape;
	sensorShape.SetAsBox((1.0f * (8/2))*1, (1.4f * (8/2))*1);

	b2FixtureDef sensorFd;
	sensorFd.shape = &sensorShape;
	sensorFd.isSensor = true;

	mStrength = 15*pDiff;
	//mBody->CreateFixture(&sensorFd);

	mHealth = 100;
}

void RangedEnemy::update() {

}


RangedEnemy::~RangedEnemy() {


}