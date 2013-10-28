#include "BombEnemy.h"

/**
* Constructor
* @param pWorld the physics world the enemy is to be added to
* @param pPos the position of the enemy
* @param pID the id number of the enemy
* @param pLoading is the enemy being loaded from a save file
* @param pDiff the games difficulty setting
* @param pPatrolA First point the enemy patrols between
* @param pPatrolB Second point the enemy patrols between
*/
BombEnemy::BombEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, bool pLoading, int pDiff, int pPatrolA, int pPatrolB):
	Enemy("Images/bombEnemy.png", pPos, pID, pDiff, pPatrolA, pPatrolB),
	mDetonated(false),
	mArmed(false),
	mSensorTtl(2) {

	float X;
	float Y;

	if(pLoading) {
		
		X = pPos.x;
		Y = pPos.y;
	}
	else { 
	
		X = (((-(768/64)*4)+(0.5*4))+(pPos.x*4)) + 0.5;
		Y = ((((1024/64)*4)-(0.5*4)) + (-pPos.y*4)) - 0.5;
	}

	mSpriteScale = 1.4;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(X, Y);
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.1f);
	mBody->SetFixedRotation(true);

	mType = "enemy";
	mSensorData = "bombEnemy";

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox((0.5f * (8/2))*mSpriteScale, (0.9f * (8/2))*mSpriteScale);

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 0.4f;
	fd.density = 10.0f;
	fd.restitution = 0.0f;
	fd.userData = mType;
	fd.filter.categoryBits = Constants::ENEMY;
	fd.filter.maskBits = Constants::PLAYER | Constants::TERRAIN | Constants::PLATFORM | Constants::TRAP;

	mBody->CreateFixture(&fd);

	b2PolygonShape sensorShape;
	sensorShape.SetAsBox((4.0f * (8/2))*mSpriteScale, (2.4f * (8/2))*mSpriteScale);

	b2FixtureDef sensorFd;
	sensorFd.shape = &sensorShape;
	sensorFd.isSensor = true;
	sensorFd.filter.categoryBits = Constants::ENEMY;
	sensorFd.filter.maskBits = Constants::PLAYER;
	sensorFd.userData = mSensorData;

	mBody->CreateFixture(&sensorFd);

	//mHealth = 100;
	mFrameHeight = 59;
	mFrameWidth = 48;
	mNumFrames = 10;
	mFrameY = 0;
	mFrameX = 0;
	mFrame = 0;
	mFrameDelay = 0;

	mDetSensorData = "explosion";
	mTtl = 50;
	mStrength = 25*pDiff;

	mBody->SetUserData(this);
}

/**
* Enemy update function
*/
void BombEnemy::update() {

	//if(mPrevPosition == mBody->GetPosition()) {
		//mState = IDLE;
	//}

	mPrevPosition = mBody->GetPosition();

	if(mState == ATTACK) {
	
		// Move towards the player
		if(mTarget.x >= mBody->GetPosition().x) {
			mDirection = 1;
			mDrawDirection = mDirection;
		}
		else {
		
			mDirection = -1;
			mDrawDirection = mDirection;
		}
	}
	else {
	
		// Idle
		mDirection = 0;
	}

	if(mDirection != 0 && mAlive) {

		b2Vec2 force = b2Vec2(150000*mDirection, 0);
		mBody->ApplyForce(force, mBody->GetWorldCenter());

		if(abs(mBody->GetLinearVelocity().x) > 7) {
			mBody->SetLinearVelocity(b2Vec2(6*mDirection, mBody->GetLinearVelocity().y));
		}
	}

	if(mArmed) {
	
		mTtl--;
		if(mTtl <= 0 && !mDetonated) {
			detonate();
		}
		else if(mDetonated) {
		
			mSensorTtl--;

			if(mSensorTtl <= 0) {
			
				if(detSensor != NULL) {
					mBody->DestroyFixture(detSensor);
					detSensor = NULL;
				}
			}
		}
	}

	draw();
}

/**
* Draws the enemy sprite
*/
void BombEnemy::draw() {

	if (mDrawDirection == 1) {
		Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_FLIP_X);
	}

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImageRegion(mImg, pos - CIwSVec2((mFrameWidth/2)*mSpriteScale, ((mFrameHeight/2)*mSpriteScale)-15), CIwSVec2(mFrameWidth*mSpriteScale, mFrameHeight*mSpriteScale), CIwSVec2(mFrame*mFrameWidth, mFrameY), CIwSVec2(mFrameWidth, mFrameHeight));

	if(mFrameDelay <= 0) {
		mFrame ++;

		if (mFrame >= mNumFrames) {

			if(mDetonated) {
			
				mAlive = false;
			}
			else {
				mFrame = 0;
			}
		}
		mFrameDelay = 5;
	}

	mFrameDelay--;

	Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_NONE);
}

/**
* Detonates the enemy
*/
void BombEnemy::detonate() {
	
	if(!mDetonated) { 

		//mBody->DestroyFixture(mBody->GetFixtureList());
		//mBody->DestroyFixture(mBody->GetFixtureList());

		mSpriteScale *= 1.4;

		b2PolygonShape detSensorShape;
		detSensorShape.SetAsBox((5.0f * (8/2))*1, (5.0f * (8/2))*1);

		b2FixtureDef detSensorFd;
		detSensorFd.shape = &detSensorShape;
		detSensorFd.isSensor = true;
		detSensorFd.userData = mDetSensorData;

		detSensor = mBody->CreateFixture(&detSensorFd);
		mFrameY = 59;
		mNumFrames = 26;

		mDetonated = true;
	}
}

/**
* Starts the enemies detonation timer countdown
*/
void BombEnemy::arm() {

	mArmed = true;
	mState = ATTACK;

	// TO DO:
		// Set animation info
}

/**
* Destructor
*/
BombEnemy::~BombEnemy() {


}