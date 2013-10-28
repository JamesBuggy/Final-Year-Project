#include "MsgSensor.h"

/**
* Constructor
* @param pPos the position of the level end sensor
* @param pSize the size of the message sensor
* @param pMsgID the id of the message
* @param pWorld the physics world to add the level end sensor to
*/
MSGSensor::MSGSensor(b2Vec2 pPos, b2Vec2 pSize, CIwStringS pMsgID, b2World* pWorld):
	msgID(pMsgID),
	mUserData("msgSensor"),
	mActivated(false) {

		float X = -(768/16) + (pSize.x*4) + (pPos.x*4) + 0.5;
		float Y = (1024/16) - (pSize.y*4) - (pPos.y*4) - 0.5;

		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(X, Y);
		
		mBody = pWorld->CreateBody(&bodyDef);

		b2PolygonShape polygonShape;
		polygonShape.SetAsBox((pSize.x * (8/2)), (pSize.y * (8/2)));

		b2FixtureDef fd;
		fd.shape = &polygonShape;
		fd.isSensor = true;
		fd.userData = mUserData;
		fd.filter.categoryBits = Constants::SENSOR;
		fd.filter.maskBits = Constants::PLAYER;

		mBody->CreateFixture(&fd);
		mBody->SetUserData(this);
}

/**
* Sets the active state of the sensor
* @param pVal the state to set as
*/
void MSGSensor::setActive(bool pVal) {

	mActivated = pVal;
}

/**
* Gets the active state
* @return the sctive state of the sensor
*/
bool MSGSensor::isActive() {

	return mActivated;
}

/**
* gets the id of the sensor
* @return the message id
*/
CIwStringS MSGSensor::getID() {

	return msgID;
}

/**
* destructor
*/
MSGSensor::~MSGSensor() {


}