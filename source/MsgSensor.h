
#ifndef MSGSENSOR_H
#define MSGSENSOR_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "Constants.h"

class MSGSensor {

public:

	MSGSensor(b2Vec2 pPos, b2Vec2 pSize, CIwStringS pMsgID, b2World* pWorld);
	~MSGSensor();

	void setActive(bool pVal);
	bool isActive();
	CIwStringS getID();

private:

	CIwStringS msgID;
	char* mUserData;
	bool mActivated;
	b2Body* mBody;
};

#endif