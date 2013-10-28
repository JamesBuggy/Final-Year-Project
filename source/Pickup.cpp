#include "Pickup.h"

int Pickup::pickupCount = 0;

/**
* Constructor
* @param pImageName the name of the image to use
* @param pPos the position of the pickup
* @param pType the type of pickup
* @param pWorld the physics world to add the pickup to
* @param pDynamic does the pickup fall under gravity
* @param pLoading is the pickup loaded from a file
*/
Pickup::Pickup(const char* pImgName, CIwSVec2 pPos, char* pType, b2World* pWorld, bool pDynamic, bool pLoading) {

	// Give unique ID
	mId = Pickup::pickupCount;
	Pickup::pickupCount++;
	CIwImage img;
	img.LoadFromFile(pImgName);
	mImg = Iw2DCreateImage(pImgName);
	mPos = CIwSVec2(pPos.x*32, (pPos.y*32));
	PickedUp = false;
	mDir = 1;
	bounceSpeed = 2;
	mDynamic = pDynamic;

	float X;// = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5;
	float Y;// = (1024/16) - (0.5*4) - (pPos.y*4) - 0.5;

	if(pLoading == true) {
		X = pPos.x;
		Y = pPos.y;
	}
	else {
	
		X = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5; // -48 + 2 + 8 + 0.5
		Y = (1024/16) - (0.5*4) - (pPos.y*4) - 0.5;
	}
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(X, Y);

	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetFixedRotation(true);

	mType = pType;
	
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.5f * (8/2), 0.5f * (8/2));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.filter.categoryBits = Constants::PICKUP;
	fd.filter.maskBits = Constants::PLAYER | Constants::KILLBOX;

	if(pDynamic == false) {
		fd.isSensor = true;
	}
	fd.userData = mType;

	mBody->CreateFixture(&fd);

	if(pDynamic == false) {
		mBody->SetGravityScale(0.0f);
	}
	mBody->SetUserData(this);

	highBounce = mBody->GetPosition().y + 0.5;
	lowBounce = mBody->GetPosition().y - 0.5;

	if(mType == "healthPickup") {
		mStrength = 20;
	}
	else if(mType == "speedPickup") {
		mStrength = 2;
	}
	else if(mType == "damagePickup") {
		mStrength = 2;
	}

	/*FileHandle = s3eFileOpen("Audio/pickup.raw", "rb");

	FileSize = s3eFileGetSize(FileHandle);
	SoundBuffer = (int16*)s3eMallocBase(FileSize);
	memset(SoundBuffer, 0, FileSize);
	s3eFileRead(SoundBuffer, FileSize, 1, FileHandle);
	s3eFileClose(FileHandle);

	// Setting default frequency at which all channels will play at, in Hz.
	s3eSoundSetInt(S3E_SOUND_DEFAULT_FREQ, 8000);*/

	mSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("pickup", IW_SOUND_RESTYPE_SPEC);
}

/**
* Updates the pickup
*/
void Pickup::update() {
	if(PickedUp == false) {	
		draw();

		if(mDynamic == false) {
			mBody->SetLinearVelocity(b2Vec2(0, bounceSpeed*mDir));

			if(mBody->GetPosition().y > highBounce) {
				mDir = -1;
			}
			else if(mBody->GetPosition().y < lowBounce) {
				mDir = 1;
			}
		}
	}
}

/**
* Gets the pickups physics body
* @return the pickups physics body
*/
b2Body* Pickup::getBody() {
	return mBody;
}

/**
* Draws the pickups image
*/
void Pickup::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImage(mImg, pos - CIwSVec2(16, 16), CIwSVec2(32, 32));
}

/**
* Gets the strength of the pickup
* @return the strength of the pickup
*/
int Pickup::getStrength() {
	return mStrength;
}

/**
* Destroys the pickups physics body
* @param pWorld the world the physics body belongs to
*/
void Pickup::destroy(b2World* pWorld) {
	pWorld->DestroyBody(mBody);
}

/**
* Gets the id number of the pickup
* @return the id number of the pickup
*/
int Pickup::getID() {

	return mId;
}

/**
* Sets the id number of the pickup
* @param pID the id number to use
*/
void Pickup::setID(int pID) {

	mId = pID;
}

/**
* sets the pikcups state to "picked up" and plays the sound effect
*/
void Pickup::pickUp() {
	PickedUp = true;

	// Finds a free channel
	Channel = s3eSoundGetFreeChannel();

	// Play sound
	/*if (s3eSoundChannelPlay(Channel, SoundBuffer, FileSize/2, 1, 0) == S3E_RESULT_ERROR) {
        sprintf(mVolumeString, "Error in s3eSoundChannelPlay");
	}*/

	mSoundSpec->Play();
}

/**
* Checks if the pickup has been picked up
* return true if picked up, else false
*/
bool Pickup::isPickedUp() {
	return PickedUp;
}

/**
* Saves the pickup to an XML file
* @param pDoc the file to save to
*/
void Pickup::save(TiXmlDocument& pDoc) {

	TiXmlElement * pickup = new TiXmlElement( "pickup" );  
	pDoc.RootElement()->LinkEndChild( pickup );

	string x = Constants::convertInt(mBody->GetPosition().x);

	TiXmlElement* xPos = new TiXmlElement( "xPos" );
	xPos->LinkEndChild( new TiXmlText( x.c_str() ));
	pickup->LinkEndChild( xPos );

	string y = Constants::convertInt(mBody->GetPosition().y);

	TiXmlElement* yPos = new TiXmlElement( "yPos" );  
	yPos->LinkEndChild( new TiXmlText( y.c_str() ));
	pickup->LinkEndChild( yPos );

	TiXmlElement* type = new TiXmlElement( "type" ); 
	if(mType == "healthPickup") {
		type->LinkEndChild( new TiXmlText( "0" ));
	}
	else if(mType == "speedPickup") {
		type->LinkEndChild( new TiXmlText( "1" ));
	}
	else if(mType == "damagePickup") {
		type->LinkEndChild( new TiXmlText( "2" ));
	}
	pickup->LinkEndChild( type );
}

/**
* Serializes the pickup info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Pickup::serialize(PacketStream* pStream) {

	/*if(PickedUp == true) {
		pStream->writeInt(1);
	}
	else {
		pStream->writeInt(2);
	}*/
	pStream->writeInt(mId);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
}

/**
* Deserializes pickup info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Pickup::deserialize(PacketStream* pStream) {

	float xPos, yPos;
	int id;
	pStream->readInt(id);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);
	/*if(picked == 1) {
		PickedUp = true;
	}
	else {
		PickedUp = false;
	}*/

	if(id == mId) {

		mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);
	}
}

/**
* Destructor
*/
Pickup::~Pickup() {

}