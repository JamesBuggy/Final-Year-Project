#include "Elevator.h"

/**
* Constructor
* @param pImage the image to use
* @param pPos the position of the elevator
* @param pWidth the width of the elevator
* @param pExt1 first point which the elevator moves between
*/
Elevator::Elevator(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt1, int pExt2, b2World* pWorld):
	mImage(Iw2DCreateImage(pImage)),
	xExtent1((1024/16) - (0.5*4) - (pExt1*4) - 0.5), 
	xExtent2((1024/16) - (0.5*4) - (pExt2*4) - 0.5),
	mActive(false),
	mDirection(1),
	mWidth(pWidth*64),
	mUserData("elevator"),
	mSpeed(10),
	mPauseTimer(0) {

		float X = -48 + (pWidth*4) + (pPos.x*4) + 0.5;
		float Y = 64 - (0.5*4) - (pPos.y*4) - 0.5;
		X = -(768/16) + (pWidth*4) + (pPos.x*4) + 0.5;
		Y = (1024/16) - (0.9*4) + (-pPos.y*4) - 0.5;
		yReset = Y;
		xReset = X;

		b2BodyDef bodyDef;
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(X, Y);
		
		mBody = pWorld->CreateBody(&bodyDef);
		mBody->SetLinearDamping(0.1f);
		mBody->SetFixedRotation(true);

		b2PolygonShape polygonShape;
		polygonShape.SetAsBox((pWidth * (8/2)), (0.5f * (8/2)));

		b2FixtureDef fd;
		fd.shape = &polygonShape;
		fd.friction = 10.0f;
		fd.density = 10.0f;
		fd.restitution = 0.0f;
		//fd.userData = mUserData;
		fd.filter.categoryBits = Constants::PLATFORM;
		fd.filter.maskBits = Constants::PLAYER | Constants::ENEMY;

		mBody->CreateFixture(&fd);

		b2PolygonShape sensorShape;
		sensorShape.SetAsBox((1.0 * (8/2)), (2.0f * (8/2)));

		b2FixtureDef sensorFd;
		sensorFd.shape = &sensorShape;
		sensorFd.isSensor = true;
		sensorFd.userData = mUserData;
		sensorFd.filter.categoryBits = Constants::PLATFORM;
		sensorFd.filter.maskBits = Constants::PLAYER;

		mBody->CreateFixture(&sensorFd);

		mBody->SetGravityScale(0.0f);
		mBody->SetUserData(this);
}

/**
* Elevator update function
*/
void Elevator::update() {
	
	if(mActive) {
		if(mPauseTimer <= 0.0f) {

			if(mDirection == 1) {
				if(mBody->GetPosition().y >= xExtent2) {

					mPauseTimer = 2;
					mDirection = -1;
				}
			}
			else {
				if(mBody->GetPosition().y <= xExtent1) {

					mPauseTimer = 2;
					mDirection = 1;
				}
			}

			mBody->SetLinearVelocity(b2Vec2( 0, mSpeed*mDirection));
			mBody->SetTransform(b2Vec2(xReset, mBody->GetPosition().y), 0.0f);
		}
		else {

			mPauseTimer -= 0.1;
		}
	}
	else {
	
		mBody->SetTransform(b2Vec2(xReset, yReset), 0.0f);
		mBody->SetType(b2_staticBody);
	}

	draw();
}

/**
* Turns the elevator on or off
*/
void Elevator::toggle() {

	if(!mActive) {
	
		mBody->SetType(b2_dynamicBody);
		mActive = true;
	}
}

/**
* Draws the elevator
*/
void Elevator::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImageRegion(mImage, CIwSVec2(pos.x-(mWidth/2), (pos.y)-16), CIwSVec2(0, 0), CIwSVec2(mWidth, mImage->GetHeight()));
}

/**
* Serializes the elevator info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Elevator::serialize(PacketStream* pStream) {

	pStream->writeFloat(mBody->GetLinearVelocity().x);
	pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
}

/**
* Deserializes elevator info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Elevator::deserialize(PacketStream* pStream) {

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
Elevator::~Elevator() {


}