
#ifndef CRATE_H
#define CRATE_H

#include "Box2D\Box2D.h"
#include "Iw2D.h"
#include "PacketStream.h"
#include "Constants.h"
#include "tinyxml.h"

class Crate {

public:
	static int crateCount;

	Crate(char* pType, CIwSVec2 pPos, b2World* pWorld, bool pLoading);
	~Crate();

	void update();
	void destroy();
	bool isAlive();
	int getID();
	void setID(int pID);
	b2Body* getBody();

	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);
	void save(TiXmlDocument& pDoc);

private:

	char* mType;
	b2Body* mBody;
	CIw2DImage* mImage;
	bool mAlive;
	int mID;
};

#endif