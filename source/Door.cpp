#include "Door.h"

/**
* Constructor
* @param pImage the image too use foor the door
* @param pPos the position of the door
* @param pWidth the width of the door
* @param pExt the point the door moves to when opening
* @param pDir the direction the door opens
* @param pWorld the physics world the door is to be added to
*/
Door::Door(const char* pImage, CIwSVec2 pPos, float pWidth, int pExt, int pDir, b2World* pWorld):
	mImage(Iw2DCreateImage(pImage)),
	xExtent((((-(768/64)*4)+(0.5*4))+(pExt*4)) + 0.5), 
	mActive(false),
	mDirection(pDir),
	mWidth(pWidth*64),
	mUserData("door"),
	mSpeed(10) {

		float X = -(768/16) + (pWidth*4) + (pPos.x*4) + 0.5;
		float Y = (1024/16) - (0.5*4) + (-pPos.y*4) - 0.5;

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

		mBody->SetGravityScale(0.0f);
		mBody->SetUserData(this);
}

/**
* Door update function. syncs the image position with the physics body position
*/
void Door::update() {

	if(mActive) {
		if(mDirection == 1) {
			if(mBody->GetPosition().x <= xExtent) {
				
				mBody->SetLinearVelocity(b2Vec2(mSpeed, 0));
			}
			else {
			
				mActive = false;
				mBody->SetLinearVelocity(b2Vec2(0, 0));
			}
		}
		else if(mDirection == -1) {
			if(mBody->GetPosition().x >= xExtent) {
				
				mBody->SetLinearVelocity(b2Vec2(-mSpeed, 0));
			}
			else {
			
				mActive = false;
				mBody->SetLinearVelocity(b2Vec2(0, 0));
			}
		}
	}
	else {
		mBody->SetType(b2_staticBody);
	}
	
	draw();
}

/**
* Opens the door
*/
void Door::toggle() {

	if(!mActive) {
		mBody->SetType(b2_dynamicBody);
		mActive = true;
	}
}

/**
* Draws the door image
*/
void Door::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImageRegion(mImage, CIwSVec2(pos.x-(mWidth/2), (pos.y)-16), CIwSVec2(0, 0), CIwSVec2(mWidth, mImage->GetHeight()));
}

/**
* Serializes the door info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Door::serialize(PacketStream* pStream) {

	pStream->writeFloat(mBody->GetLinearVelocity().x);
	pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
}

/**
* Deserializes door info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Door::deserialize(PacketStream* pStream) {

	float xVel, yVel, xPos, yPos;
	pStream->readFloat(xVel);
	pStream->readFloat(yVel);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);

	mBody->SetLinearVelocity(b2Vec2(xVel,yVel));
	mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);
}
