
#include "Crate.h"

int Crate::crateCount = 0;

/**
* Constructor
* @param pType the type of crate to create
* @param pPos the position of the crate
* @param pLoading is the crate being loaded from a save file
*/
Crate::Crate(char* pType, CIwSVec2 pPos, b2World* pWorld, bool pLoading) {

	mID = Crate::crateCount;
	Crate::crateCount++;

	mType = pType;
	mAlive = true;

	float X; // = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5;
	float Y; // = (1024/16) - (0.5*4) - (pPos.y*4) - 0.5;

	if(pLoading == true) {
		X = pPos.x;
		Y = pPos.y;
	}
	else {
	
		X = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5;
		Y = (1024/16) - (0.5*4) - (pPos.y*4) - 0.5;
	}

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(X, Y);
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.5f);
	mBody->SetFixedRotation(true);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox((1.0f * (8/2)), (1.0f * (8/2)));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 0.2f;
	fd.density = 5.0f;
	fd.restitution = 0.2f;
	fd.userData = mType;
	fd.filter.categoryBits = Constants::OBJECT;
	fd.filter.maskBits = Constants::PLAYER | Constants::TERRAIN | Constants::PLATFORM | Constants::TRAP;

	mBody->CreateFixture(&fd);

	if(pType == "metalCrate") {
		mImage = Iw2DCreateImage("Images/metalCrate.png");
	}
	else if(pType == "woodenCrate") {
		mImage = Iw2DCreateImage("Images/woodenCrate.png");
	}

	mBody->SetUserData(this);
}

/**
* Crate update function
*/
void Crate::update() {

	static const CIwSVec2 imageSize(mImage->GetWidth() >> 3, mImage->GetHeight() >> 3);
	static const CIwSVec2 halfImageSize = CIwSVec2(mImage->GetWidth()/2, mImage->GetHeight()/2);

	const CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	const b2Transform t = mBody->GetTransform();
	const CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
	const float angle = -t.q.GetAngle() * (180.0f/3.14159f);	// reverse angle as our screen-Y coord is reversed
	
	/*CIwMat2D temp = Iw2DGetTransformMatrix();
	CIwMat2D rot;
	rot.SetRot(iwangle(angle * 1024 / 90), CIwVec2(pos) << 3);
	Iw2DSetTransformMatrixSubPixel(rot);*/

	Iw2DDrawImage(mImage, pos - halfImageSize, CIwSVec2(mImage->GetWidth(), mImage->GetHeight()));

	//Iw2DSetTransformMatrix(temp);
}

/**
* destroys the crate
*/
void Crate::destroy() {

	mAlive = false;
}

/**
* Gets the crates status
* @return the crates status
*/
bool Crate::isAlive() {
	return mAlive;
}

/**
* gets the crates physics body
* @return the crates physics body
*/
b2Body* Crate::getBody() {
	return mBody;
}

/**
* Get the id number of the crate
* @return the crates id number
*/
int Crate::getID() {
	return mID;
}

/**
* Sets the crates id number
* @param pID the id number to set as
*/
void Crate::setID(int pID) {
	mID = pID;
}

/**
* Saves the crate to an XML file
* @param pDoc the file to save to
*/
void Crate::save(TiXmlDocument& pDoc) {

	TiXmlElement * crate = new TiXmlElement( "crate" );  
	pDoc.RootElement()->LinkEndChild( crate );

	string x = Constants::convertInt(mBody->GetPosition().x);

	TiXmlElement* xPos = new TiXmlElement( "xPos" );
	xPos->LinkEndChild( new TiXmlText( x.c_str() ));
	crate->LinkEndChild( xPos );

	string y = Constants::convertInt(mBody->GetPosition().y);

	TiXmlElement* yPos = new TiXmlElement( "yPos" );  
	yPos->LinkEndChild( new TiXmlText( y.c_str() ));
	crate->LinkEndChild( yPos );

	TiXmlElement* type = new TiXmlElement( "type" ); 
	if(mType == "woodenCrate") {
		type->LinkEndChild( new TiXmlText( "0" ));
	}
	else if(mType == "metalCrate") {
		type->LinkEndChild( new TiXmlText( "1" ));
	}
	crate->LinkEndChild( type );
}

/**
* Serializes the crate info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Crate::serialize(PacketStream* pStream) {

	//pStream->writeInt(mID);
	//pStream->writeFloat(mBody->GetLinearVelocity().x);
	//pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
}

/**
* Deserializes crate info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Crate::deserialize(PacketStream* pStream) {

	float xVel, yVel, xPos, yPos;
	int id;
	//pStream->readInt(id);
	//pStream->readFloat(xVel);
	//pStream->readFloat(yVel);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);

	//if(mID == id) {
		
		//mBody->SetLinearVelocity(b2Vec2(xVel,yVel));
	mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);
	//}
}

/**
* Destructor
*/
Crate::~Crate() {

}