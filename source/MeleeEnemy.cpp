#include "MeleeEnemy.h"

/**
* Constructor
* @param pWorld the physics world to add the level end sensor to
* @param pPos the position of the level end sensor
* @param pID the id number of the enemy
* @param pDiff the difficulty level of the game
* @param pPatrolA first point which the enemy patrols between
* @param pPatrolB second point the enemy patrols between
*/
MeleeEnemy::MeleeEnemy(b2World* pWorld, CIwSVec2 pPos, int pID, bool pLoading, int pDiff, int pPatrolA, int pPatrolB):
	Enemy("Images/meleeEnemy.png", pPos, pID, pDiff, pPatrolA, pPatrolB),
	mAttackRange(10),
	mVerFrame(0),
	mFrameWidth(72),
	mFrameCount(9),
	mPunchTimer(0) {

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

	mSpriteScale = 1;
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(X, Y);
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.1f);
	mBody->SetFixedRotation(true);
	
	mType = "enemy";
	mSensorData = "meleeEnemy";
	
	b2PolygonShape polygonShape;
	//polygonShape.SetAsBox((0.5f * (8/2))*mSpriteScale, (0.6f * (8/2))*mSpriteScale);

	b2Vec2 vertices[6];
	vertices[0].Set(-1.25, -3);
	vertices[1].Set(1.25, -3);
	vertices[2].Set(2.5, -2.8);
	vertices[3].Set(2.5, 3);
	vertices[4].Set(-2.5, 3);
	vertices[5].Set(-2.5, -2.8);

	polygonShape.Set(vertices, 6);

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 0.4f;
	fd.density = 10.0f;
	fd.restitution = 0.0f;
	fd.filter.categoryBits = Constants::ENEMY;
	fd.filter.maskBits = Constants::PLAYER | Constants::TERRAIN | Constants::PLATFORM | Constants::TRAP;
	fd.userData = mType;

	mBody->CreateFixture(&fd);

	b2PolygonShape sensorShape;
	sensorShape.SetAsBox((4.0f * (8/2))*1, (1.4f * (8/2))*1);

	b2FixtureDef sensorFd;
	sensorFd.shape = &sensorShape;
	sensorFd.isSensor = true;
	sensorFd.userData = mSensorData;
	sensorFd.filter.categoryBits = Constants::ENEMY;
	sensorFd.filter.maskBits = Constants::PLAYER;

	mBody->CreateFixture(&sensorFd);

	mStrength = 20*pDiff;
	mFrame = 0;
	mFrameDelay = 0;
	punching = false;
	mDirection = 1;

	mBody->SetUserData(this);
}
	
/**
* Updates the melee enemy
* @param of this game is host or not
*/
void MeleeEnemy::update(bool pHost) {

	b2Vec2 target = mPlayer->getBody()->GetPosition();

	if(abs(target.x - mBody->GetPosition().x) <= 35 && abs(target.y - mBody->GetPosition().y) <= 5) {
		mState = ATTACK;
	}
	else {
		mState = IDLE;
	}

	if(mCanMove == true) {

		if(mPunchTimer > 0) {
			mPunchTimer -= 0.1f;
		}

		if(mState == ATTACK) {

			// Move towards the player
			if(target.x >= mBody->GetPosition().x) {

				mDirection = 1;
				mDrawDirection = mDirection;

				if(abs((target - mBody->GetPosition()).Length() <= mAttackRange)) {
					mDirection = 0;

					//setAttack();

					if(punching == false) {

						if(mPunchTimer <= 0.0f) {
							punch();
						}
					}
				}
			}

			if(target.x < mBody->GetPosition().x) {
		
				mDirection = -1;
				mDrawDirection = mDirection;

				if(abs((target - mBody->GetPosition()).Length() <= mAttackRange)) {
					mDirection = 0;

					//setAttack();

					if(punching == false) {
						if(mPunchTimer <= 0.0f) {
							punch();
						}
					}
				}
			}

			if(mFrame == mFrameWidth*8 && abs((target - mBody->GetPosition()).Length() <= mAttackRange)) {
				
				if(punching) {
					// hurt player
					mPlayer->applyDamage(mStrength);
					punching = false;
				}
			}

			//if(abs(mTarget.x - mBody->GetPosition().x >= mAttackRange*10)) {

				//mState = IDLE;
			//}
		}
		else if(mState == IDLE && mAlive) {

			if(mBody->GetPosition().x >= mPatrolPointB) {

				mDirection = -1;
				mDrawDirection = mDirection;
			}
			else if(mBody->GetPosition().x <= mPatrolPointA) {

				mDirection = 1;
				mDrawDirection = mDirection;
			}
		}

		b2Vec2 force = b2Vec2(150000*mDirection, 0);
		mBody->ApplyForce(force, mBody->GetWorldCenter());

		if(abs(mBody->GetLinearVelocity().x) > 7) {
			mBody->SetLinearVelocity(b2Vec2(6*mDirection, mBody->GetLinearVelocity().y));
		}
	}
	else {
		mStunTimer -= 0.1;

		if(mStunTimer <= 0.0f) {
			mCanMove = true;
		}
	}

	
	if(pHost) {
		if(mHealth <= 0) {

			mAlive = false;
			mDeathSoundSpec->Play();
		}
	}

	draw();
}

/**
* Sets the current state of the enemy to attack
*/
void MeleeEnemy::setAttack() {

	mState = ATTACK;
	/*punching = true;
	mFrame = 0;
	mFrameCount = 11;*/
}

/**
* Sets the current state of the enemy to idle
*/
void MeleeEnemy::setIdle() {

	mState = IDLE;
	punching = false;
	mFrameCount = 9;
	mVerFrame = 0;
	mFrame = 0;	
}

/**
* Draws the enemy image
*/
void MeleeEnemy::draw() {

	if (mDrawDirection == 1) {
		Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_FLIP_X);
	}

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	float normalRegionHeight = (52.0f/100.0f) * ((mHealth/mMaxHealth)*100.0f);
	float hurtRegionHeight = 52.0f - normalRegionHeight;

	CIwColour col;
	col.Set(255, 50, 50);
	Iw2DSetColour(col);
	Iw2DDrawImageRegion(mImg, pos - CIwSVec2((mFrameWidth/1.5)*mSpriteScale, ((52/2)*mSpriteScale)), CIwSVec2(mFrameWidth*1.5, hurtRegionHeight*mSpriteScale), CIwSVec2(mFrame, mVerFrame), CIwSVec2(mFrameWidth, hurtRegionHeight));

	col.Set(255, 255, 255);
	Iw2DSetColour(col);
	Iw2DDrawImageRegion(mImg, pos - CIwSVec2((mFrameWidth/1.5)*mSpriteScale, (((52/2)*mSpriteScale))-hurtRegionHeight), CIwSVec2(mFrameWidth*1.5, normalRegionHeight*mSpriteScale), CIwSVec2(mFrame, hurtRegionHeight+mVerFrame), CIwSVec2(mFrameWidth, normalRegionHeight));

	if(mFrameDelay <= 0) {
		mFrame += (mFrameWidth);

		if (mFrame > (mFrameCount*mFrameWidth)) {

			//setIdle();
			mFrame = 0;
			if(punching) {
				setIdle();
				mPunchTimer = 10.0f;
			}
		}
		mFrameDelay = 5;
	}

	mFrameDelay--;

	Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_NONE);
}

/**
* Start the enemies attack
*/
void MeleeEnemy::punch() {

	punching = true;
	mFrame = 0;
	mFrameCount = 10;
	mVerFrame = 52;
	mPunchTimer = 10.0f;
}

/**
* destructor
*/
MeleeEnemy::~MeleeEnemy() {


}