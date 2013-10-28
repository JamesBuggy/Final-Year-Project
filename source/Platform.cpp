#include "Platform.h"

/**
* Constructor
* @param pImage the name of the image to use
* @param pPos the position of the platform
* @param pWidth the width of the platform
* @param pExt1 the first of two points the platform moves between
* @param pExt2 the second of two points the platform moves between
* @param pWorld the physics world to add the pickup to
*/
Platform::Platform(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt1, int pExt2, b2World* pWorld):
	mImage(Iw2DCreateImage(pImage)),
	xExtent1((((-(768/64)*4)+(0.5*4))+(pExt1*4)) + 0.5), 
	xExtent2((((-(768/64)*4)+(0.5*4))+(pExt2*4)) + 0.5),
	mSpeed(10) {

		mDirection = 1;
		mWidth = pWidth*64;

		float X = -48 + (pWidth*4) + (pPos.x*4) + 0.5;
		float Y = 64 - (0.5*4) - (pPos.y*4) - 0.5;
		X = -(768/16) + (pWidth*4) + (pPos.x*4) + 0.5;
		Y = (1024/16) - (0.9*4) + (-pPos.y*4) - 0.5;
		yReset = Y;

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(X, Y);
		mBody = pWorld->CreateBody(&bodyDef);
		mBody->SetLinearDamping(0.1f);
		mBody->SetFixedRotation(true);

		b2PolygonShape polygonShape;
		polygonShape.SetAsBox((pWidth * (8/2)), (0.5f * (8/2)));

		mType = "platform";

		b2FixtureDef fd;
		fd.shape = &polygonShape;
		fd.friction = 10.0f;
		fd.density = 10.0f;
		fd.restitution = 0.0f;
		fd.userData = mType;
		fd.filter.categoryBits = Constants::PLATFORM;
		fd.filter.maskBits = Constants::PLAYER | Constants::ENEMY;

		mBody->CreateFixture(&fd);
		mBody->SetGravityScale(0.0f);
		mBody->SetUserData(this);
}

/**
* Updates the platform
*/
void Platform::update() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
	
	if(mDirection == 1) {
		if(mBody->GetPosition().x >= xExtent2) {
			mDirection *= -1;
		}
	}
	else {
		if(mBody->GetPosition().x <= xExtent1) {
			mDirection *= -1;
		}
	}

	mBody->SetLinearVelocity(b2Vec2( mSpeed*mDirection, 0));
	mBody->SetTransform(b2Vec2(mBody->GetPosition().x, yReset), 0.0f);

	draw();
}

/**
* Draws the platform image
*/
void Platform::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImageRegion(mImage, CIwSVec2(pos.x-(mWidth/2), (pos.y)-16), CIwSVec2(0, 0), CIwSVec2(mWidth, mImage->GetHeight()));
}

/**
* Serializes the platform info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Platform::serialize(PacketStream* pStream) {

	pStream->writeFloat(mBody->GetLinearVelocity().x);
	pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
}

/**
* Deserializes platform info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Platform::deserialize(PacketStream* pStream) {

	float xVel, yVel, xPos, yPos;
	pStream->readFloat(xVel);
	pStream->readFloat(yVel);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);

	mBody->SetLinearVelocity(b2Vec2(xVel,yVel));
	mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);
}

/**
* Destructor
*/
Platform::~Platform() {


}