
#include "LevelEnd.h"

/**
* Constructor
* @param pPos the position of the level end sensor
* @param pSize the size of the level end sensor
* @param pImageName the image to use
* @param pWorld the physics world to add the level end sensor to
*/
LevelEnd::LevelEnd(b2Vec2 pPos, b2Vec2 pSize, const char* pImageName, b2World* pWorld):

	mUserData("levelEnd"),
	mActive(false) {


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
	fd.filter.maskBits = Constants::PLAYER;
	fd.userData = mUserData;

	mBody->CreateFixture(&fd);

	mImage = Iw2DCreateImage(pImageName);
	mBody->SetUserData(this);
}

/**
* Draws the level end image
*/
void LevelEnd::draw() {
	
	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImage(mImage, pos-CIwSVec2(mImage->GetWidth()/2, mImage->GetHeight()/2));
}

/**
* sets the sensor active state
*/
void LevelEnd::setActive() {

	mActive = true;	
}

/**
* Gets the sensor active state
* @return the sensors active state
*/
bool LevelEnd::isActive() {

	return mActive;
}