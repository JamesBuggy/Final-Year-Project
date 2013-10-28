#include "player.h"

/**
* Constructor
* @param pImage the name of the image to use
* @param pPos the position of the player
* @param pWorld the physics world to add the pickup to
* @param pLoading if the player is loaded from a file or not
*/
Player::Player(const char* pImage, b2Vec2 pPos, b2World* pWorld, bool pLoading):
	jumped(0),
	direction(0),
	prevDir(1),
	frameDelayLegs(0),
	frameDelayTorso(0),
	grabbedWall(false),
	wallJumpDir(0),
	scale(1.5),
	collWait(0),
	mMeleeStr(20),
	punching(false),
	kicking(false),
	canRun(true),
	mHealth(100),
	mBaseMeleeStr(mMeleeStr),
	mSpeedMult(1),
	mSpeedBoostCounter(0),
	mDmgBoostCounter(0),
	mOnPlatform(false),
	mFinished(false) { 

	frameHeight = 52;
	frameWidth = 56;

	// Torso frame info
	curFrameTorso = 0;
	numFramesTorso = 1;
	frameXTorso = 0;
	frameYTorso = 104;

	// Leg frame info
	curFrameLegs = 0;
	numFramesLegs = 12;
	frameXLegs = 0;
	frameYLegs = 0;

	maxFrameDelay = 3;

	float X;
	float Y;

	if(pLoading) {
		
		X = pPos.x;
		Y = pPos.y;
	}
	else { 
		//int i = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
		X = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5; 
		Y = (1024/16) - (0.9*4) - (pPos.y*4) - 0.5;
	}

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(X, Y);
	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.15f);
	mBody->SetFixedRotation(true);
	mBody->SetGravityScale(2);
	mType = "player";

	//b2PolygonShape polygonShape;

	b2Vec2 vertices[6];
	vertices[0].Set(-1.25, -4);
	vertices[1].Set(1.25, -4);
	vertices[2].Set(2.5, -3.8);
	vertices[3].Set(2.5, 4);
	vertices[4].Set(-2.5, 4);
	vertices[5].Set(-2.5, -3.8);
  
	b2PolygonShape polygonShape;
	polygonShape.Set(vertices, 6);

	//polygonShape.SetAsBox((0.5f * (8/2))*1, (0.9f * (8/2))*1);

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 0.4f;
	fd.density = 15.0f;
	fd.restitution = 0.0f;
	fd.filter.categoryBits = Constants::PLAYER;
	fd.filter.maskBits = Constants::ENEMY | Constants::TERRAIN | Constants::PLATFORM | Constants::TRAP | Constants::SENSOR;
	fd.userData = mType;

	mFixture = mBody->CreateFixture(&fd);

	CIwImage img;
	img.LoadFromFile(pImage);
	mImage = Iw2DCreateImage(img);

	mBody->SetLinearDamping(0.1f);
	mBody->SetUserData(this);

	//alive = true;
	loadCheckpoint = false;


	CIwImage speedImg;
	speedImg.LoadFromFile("Images/speedBoost.png");
	mSpeedIcon = Iw2DCreateImage(speedImg);

	CIwImage dmgImg;
	dmgImg.LoadFromFile("Images/damageBoost.png");
	mDamageIcon = Iw2DCreateImage(dmgImg);

	// Read in kick sound data
	/*kickFileHandle = s3eFileOpen("Audio/kick_2.raw", "rb");

	kickFileSize = s3eFileGetSize(kickFileHandle);
	kickSoundBuffer = (int16*)s3eMallocBase(kickFileSize);
	memset(kickSoundBuffer, 0, kickFileSize);
	s3eFileRead(kickSoundBuffer, kickFileSize, 1, kickFileHandle);
	s3eFileClose(kickFileHandle);

	// Finds a free channel
	//kickChannel = s3eSoundGetFreeChannel();

	// Read in punch sound data
	punchFileHandle = s3eFileOpen("Audio/kick_1.raw", "rb");

	punchFileSize = s3eFileGetSize(punchFileHandle);
	punchSoundBuffer = (int16*)s3eMallocBase(punchFileSize);
	memset(punchSoundBuffer, 0, punchFileSize);
	s3eFileRead(punchSoundBuffer, punchFileSize, 1, punchFileHandle);
	s3eFileClose(punchFileHandle);

	// Finds a free channel
	//punchChannel = s3eSoundGetFreeChannel();

	// Setting default frequency at which all channels will play at, in Hz.
	s3eSoundSetInt(S3E_SOUND_DEFAULT_FREQ, 8000);*/

	//punchSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("swing", IW_SOUND_RESTYPE_SPEC);
	//kickSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("kick", IW_SOUND_RESTYPE_SPEC);
	
	mKickSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("kick_1", IW_SOUND_RESTYPE_SPEC));
	mKickSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("kick_2", IW_SOUND_RESTYPE_SPEC));
	mKickSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("kick_3", IW_SOUND_RESTYPE_SPEC));

	mPunchSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("swing_1", IW_SOUND_RESTYPE_SPEC));
	mPunchSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("swing_2", IW_SOUND_RESTYPE_SPEC));
	mPunchSounds.push_back((CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("swing_3", IW_SOUND_RESTYPE_SPEC));

	mSpawnSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("respawn", IW_SOUND_RESTYPE_SPEC);

	srand((unsigned)time(0));
}

/**
* Gets the physics body of the player
* @return the players physics body
*/
b2Body* Player::getBody() {

	return mBody;
}

/**
* gets the player health
* @return the player health
*/
int Player::getHealth() {
	return mHealth;
}

/**
* Applies a force to the players physics body
* @param pForce the force to apply
*/
void Player::accelerate(b2Vec2 pForce) {

	mBody->ApplyForce(pForce, mBody->GetWorldCenter());
}

/**
* The players update function
* @param pDeltaTime the time since the last frame
*/
void Player::update(float pDeltaTime) {

	if(mFinished == false) {

		CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

		b2Transform t = mBody->GetTransform();
		CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

		if(loadCheckpoint) {
	
			mBody->SetTransform(b2Vec2(mPos.x, mPos.y), 0.0f);
			loadCheckpoint = false;
			mHealth = 100;
			mSpeedBoostCounter = 0;
			mDmgBoostCounter = 0;
			mSpawnSoundSpec->Play();
			//alive = true;
		}

		if(mSpeedBoostCounter <= 0) {
			mSpeedMult = 1;
		}
		else {
			float regionHeight = (85.0f/100.0f)*(20.0f * mSpeedBoostCounter);
			Iw2DDrawImageRegion(mSpeedIcon, CIwSVec2((pos.x+(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)) - mSpeedIcon->GetWidth(), (pos.y-(s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2) +mSpeedIcon->GetHeight() - regionHeight)), CIwSVec2(0, mSpeedIcon->GetHeight() - regionHeight), CIwSVec2(mSpeedIcon->GetWidth(), regionHeight) );
		}
		if(mDmgBoostCounter <= 0) {
			mMeleeStr = mBaseMeleeStr;
		}
		else {
			float regionHeight = (85.0f/100.0f)*(20.0f * mDmgBoostCounter);
			Iw2DDrawImageRegion(mDamageIcon, CIwSVec2((pos.x+(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)) - mSpeedIcon->GetWidth(), (pos.y-(s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2) + mSpeedIcon->GetHeight() + mDamageIcon->GetHeight() - regionHeight)), CIwSVec2(0, mDamageIcon->GetHeight() - regionHeight), CIwSVec2(mDamageIcon->GetWidth(), regionHeight) );
		}

		if(collWait == 0) {
			b2ContactEdge* edge = mBody->GetContactList();
	
			b2WorldManifold mWorldMan;

			for(; edge; edge = edge->next) {
				edge->contact->GetWorldManifold(&mWorldMan);

				if(edge->contact->IsTouching()) {
					b2Vec2 norm = mWorldMan.normal;
					norm.Normalize();

					if(edge->contact->GetFixtureA()->GetUserData() == "ground" ||
						edge->contact->GetFixtureB()->GetUserData() == "ground") {

							if(norm.y == 1) { 
								// landed on ground
								if(jumped) {
									idleLegs();
									canRun = true;
									//direction = 0;
								}
								jumped = 0;
								grabbedWall = false;
								wallJumpDir = 0;				
							}
							else if(norm.y == -1) { 
								// hit roof
							}
							else if(norm.x == 1 ) {
								// hit left wall
								grabbedWall = true;
								wallJumpDir = 1;
								jumped = 0;
								wallGrab();
								canRun = true;
							}
							else if(norm.x == -1 ) {
								// hit right wall
								grabbedWall = true;
								wallJumpDir = -1;
								jumped = 0;
								wallGrab();
								canRun = true;
							}
					}
				}		
			}

			edge = mBody->GetContactList();
			if(!edge) {
				grabbedWall = false;
				wallJumpDir = 0;

				curFrameLegs = 0;
				numFramesLegs = 1;
				frameXLegs = 0;
				frameYLegs = 260;
			}
		}

		if(grabbedWall == true) {
			//mBody->SetLinearVelocity(b2Vec2(0, -2));
		}
		else if(direction != 0) {
			b2Vec2 force = b2Vec2((450000*direction)*mSpeedMult, 0);
			mBody->ApplyForce(force, mBody->GetWorldCenter());

			if(abs(mBody->GetLinearVelocity().x) > 6) {
				mBody->SetLinearVelocity(b2Vec2(5*direction, mBody->GetLinearVelocity().y));
			}
			prevDir = direction;
		}

		if (prevDir == -1) {
			Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_FLIP_X);
		}

		CIwSVec2 drawPosition = pos - CIwSVec2((frameWidth/2)*scale, (frameHeight-22)*scale);

		float normalRegionHeight = ((frameHeight/100)*mHealth);
		float hurtRegionHeight = (frameHeight - normalRegionHeight)+7;
		normalRegionHeight -= 7;

		if(mHealth == 100) {
		
			Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, frameHeight*scale), CIwSVec2(frameXLegs + (curFrameLegs*frameWidth), frameYLegs), CIwSVec2(frameWidth, frameHeight));
	
			if(!kicking && !grabbedWall){

				Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, frameHeight*scale), CIwSVec2(frameXTorso + (curFrameTorso*frameWidth), frameYTorso), CIwSVec2(frameWidth, frameHeight));	
			}
		}
		else {
			CIwColour col;
			col.Set(255, 50, 50);
			Iw2DSetColour(col);
			Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, hurtRegionHeight*scale), CIwSVec2(frameXLegs + (curFrameLegs*frameWidth), frameYLegs), CIwSVec2(frameWidth, hurtRegionHeight));
	
			col.Set(255, 255, 255);
			Iw2DSetColour(col);
			Iw2DDrawImageRegion(mImage, CIwSVec2(drawPosition.x, drawPosition.y+hurtRegionHeight*scale), CIwSVec2(frameWidth*scale, normalRegionHeight*scale), CIwSVec2(frameXLegs + (curFrameLegs*frameWidth), frameYLegs+hurtRegionHeight+1), CIwSVec2(frameWidth, normalRegionHeight));

			if(!kicking && !grabbedWall){

				col.Set(255, 50, 50);
				Iw2DSetColour(col);
				Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, hurtRegionHeight*scale), CIwSVec2(frameXTorso + (curFrameTorso*frameWidth), frameYTorso), CIwSVec2(frameWidth, hurtRegionHeight));	
	
				col.Set(255, 255, 255);
				Iw2DSetColour(col);
				Iw2DDrawImageRegion(mImage, CIwSVec2(drawPosition.x, drawPosition.y+(hurtRegionHeight*scale)), CIwSVec2(frameWidth*scale, normalRegionHeight*scale), CIwSVec2(frameXTorso + (curFrameTorso*frameWidth), frameYTorso+hurtRegionHeight), CIwSVec2(frameWidth, normalRegionHeight));	
			}

			col.Set(255, 255, 255);
			Iw2DSetColour(col);
		}

		Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_NONE);
		
		if(abs(mBody->GetLinearVelocity().x) < 0.1 || mOnPlatform == true && direction == 0) { // 0.1
			// Update idle animation
			if(direction == 0 && !kicking && jumped == 0) {
				idleLegs();
			}
		}
		else {

			if(!kicking && jumped == 0){
				numFramesLegs = 12;
				frameYLegs = 0;
			}

			// Update running animation
			if(frameDelayLegs <= 0) {
				curFrameLegs++;

				if(curFrameLegs >= numFramesLegs) {

					curFrameLegs = frameXLegs;
				}

				frameDelayLegs = maxFrameDelay;
			}	
		}

		// Update kick anim
		if(kicking) {
			if(frameDelayLegs <= 0) {
				curFrameLegs++;

				if(curFrameLegs >= numFramesLegs) {

					//curFrameLegs = frameXLegs;
					goIdle();
				}

				frameDelayLegs = maxFrameDelay;
			}	
		}
	
		// Update torso anim
		if(frameDelayTorso <= 0) {
			curFrameTorso++;

			if(curFrameTorso >= numFramesTorso) {

				if(punching == false) {
					curFrameTorso = frameXTorso;
				}
				else {
					idleTorso();
				}
			}	

			frameDelayTorso = maxFrameDelay;
		}

		if(collWait > 0) {
			collWait--;
		}

		frameDelayTorso--;
		frameDelayLegs--;

		mSpeedBoostCounter -= 1* pDeltaTime;
		mDmgBoostCounter -= 1* pDeltaTime;

		//mPos = pos - CIwSVec2(frameWidth/2, frameHeight/2);
	}
}

/**
* Draws the player image
*/
void Player::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	if (prevDir == -1) {
		Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_FLIP_X);
	}

	CIwSVec2 drawPosition = pos - CIwSVec2((frameWidth/2)*scale, (frameHeight-17)*scale);

	Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, frameHeight*scale), CIwSVec2(frameXLegs + (curFrameLegs*frameWidth), frameYLegs), CIwSVec2(frameWidth, frameHeight));
	
	if(!kicking){
		Iw2DDrawImageRegion(mImage, drawPosition, CIwSVec2(frameWidth*scale, frameHeight*scale), CIwSVec2(frameXTorso + (curFrameTorso*frameWidth), frameYTorso), CIwSVec2(frameWidth, frameHeight));	
	}

	Iw2DSetImageTransform(IW_2D_IMAGE_TRANSFORM_NONE);
}

/**
* Activates the players kick ability
*/
void Player::kick() {
	
	if(abs(mBody->GetLinearVelocity().y) < 0.1 && !kicking) {

		mBody->SetLinearVelocity(b2Vec2(0, 0));
		direction = 0;

		curFrameLegs = 0;
		numFramesLegs = 6;
		frameXLegs = 0;
		frameYLegs = 156;

		canRun = false;
		kicking = true;

		// Finds a free channel
		//kickChannel = s3eSoundGetFreeChannel();

		// Play sound
		/*if (s3eSoundChannelPlay(0, kickSoundBuffer, kickFileSize/2, 1, 0) == S3E_RESULT_ERROR) {
            sprintf(mVolumeString, "Error in s3eSoundChannelPlay");
		}*/

		// Play the sound effect
		int random_integer = (rand()%mKickSounds.size()); 
		mKickSounds.at(random_integer)->Play();
	}
}

/**
* Sets if the player can run or not
* @param pVal if the player can run or not
*/
void Player::setCanRun(bool pVal) {

	canRun = pVal;
}

/**
* Activates the players wall grab ability
*/
void Player::wallGrab() {

	curFrameLegs = 0;
	numFramesLegs = 1;
	frameXLegs = 0;
	frameYLegs = 312;
}

/**
* Activates the players punch ability
*/
void Player::punch() {

	if(!punching && !kicking) {
		curFrameTorso = 0;
		numFramesTorso = 6;
		frameXTorso = 0;
		frameYTorso = 52;
		punching = true;

		// Finds a free channel
		/*punchChannel = s3eSoundGetFreeChannel();

		// Play sound
		if (s3eSoundChannelPlay(punchChannel, punchSoundBuffer, punchFileSize/2, 1, 0) == S3E_RESULT_ERROR) {
            sprintf(mVolumeString, "Error in s3eSoundChannelPlay");
		}*/

		//punchChannel = s3eSoundGetFreeChannel();

		// Play sound
		/*if (s3eSoundChannelPlay(0, punchSoundBuffer, punchFileSize/2, 1, 0) == S3E_RESULT_ERROR) {
            sprintf(mVolumeString, "Error in s3eSoundChannelPlay");
		}*/

		// Play the sound effect
		int random_integer = (rand()%mPunchSounds.size()); 
		mPunchSounds.at(random_integer)->Play();
	}
}

/**
* Sets the player state to idle
*/
void Player::goIdle() {

	idleLegs();
	idleTorso();
}

/**
* sets the players leg animation to the idle state
*/
void Player::idleLegs() {

	// Leg frame info
	curFrameLegs = 0;
	numFramesLegs = 1;
	frameXLegs = 0;
	frameYLegs = 208;
	canRun = true;
	kicking = false;
}

/**
* Sets the players torso animation to the idle state
*/
void Player::idleTorso() {

	// Torso frame info
	curFrameTorso = 0;
	numFramesTorso = 1;
	frameXTorso = 0;
	frameYTorso = 104;
	punching = false;
}

void Player::kill() {


}

/**
* Flags the player as having finished the level
*/
void Player::finish() {
	mFinished = true;
}

/**
* Checks if the player has finished the level
* @return true if player has finished the current level, else false
*/
bool Player::isFinished() {
	return mFinished;
}

/**
* Applies damage boost to the player
* @param pVal the strength of the boost
*/
void Player::damageBoost(int pVal) {
	mMeleeStr *= pVal;
	mDmgBoostCounter = 5;
}

/**
* Applies speed boost to the player
* @param pVal the strength of the boost
*/
void Player::speedBoost(int pVal) {
	mSpeedMult = pVal;
	mSpeedBoostCounter = 5;
}

/**
* Heals the player
* @param pVal the amount of health to add
*/
void Player::heal(int pVal) {
	
	if(mHealth < 100) {
		mHealth += pVal;

		if(mHealth > 100) {
			mHealth = 100;
		}
	}
}

/**
* Activates the players jump ability
*/
void Player::jump() {

	if(canRun) {
		jumped++;

		if(jumped <= 1) {
			mBody->SetLinearVelocity(b2Vec2(mBody->GetLinearVelocity().x, 0));

			curFrameLegs = 0;
			numFramesLegs = 1;
			frameXLegs = 0;
			frameYLegs = 260;

			if(grabbedWall == true) {
				direction = wallJumpDir;
				accelerate(b2Vec2(100000*wallJumpDir, 900000));
				wallJumpDir = 0;
				grabbedWall = false;
				collWait = 3;
			}
			else {
				accelerate(b2Vec2(0, 800000));

				/*b2RayCastInput input;
				b2RayCastOutput output;

				input.p1 = mBody->GetPosition();
				input.p2 = b2Vec2(mBody->GetPosition().x, mBody->GetPosition().y+1);
				input.maxFraction = 500;

				if(mFixture->RayCast(&output, input, 0) == true) {
					accelerate(b2Vec2(0, 800000));
				}*/
			}
		}
	}
}

/**
* Sets the players movement direction
* @param pDir the direction to use
*/
void Player::setDir(int pDir) {

	if(canRun) {
		direction = pDir;

		if(grabbedWall == true) {
			if(direction == wallJumpDir) {
				grabbedWall = false;
				wallJumpDir = 0;

				collWait = 3;
			}
		}
	}
}

/**
* Applies damage to the player
* @param pDamage the amount of damage to apply
*/
void Player::applyDamage(int pDamage) {

	mHealth -= pDamage;
}

/**
* gets the player image
* @return the players image
*/
CIw2DImage* Player::getImage() {
	return mImage;
}

/**
* gets the players position
* @return the players position
*/
CIwSVec2 Player::getPos() {

	return mPos;
}

/**
* Gets thew players melee strength
* @param the players melee strength
*/
int Player::getMeleeStr() {

	return mMeleeStr;
}

/**
* Sets flag to say if the player is on a platform
* @param pVal the value with which to set the flag
*/
void Player::onPlatform(bool pVal) {
	mOnPlatform = pVal;
}

/**
* Serializes the player info to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Player::serialize(PacketStream* pStream) {


	/*CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x), -int16(t.p.y)));*/

	pStream->writeFloat(mBody->GetLinearVelocity().x);
	pStream->writeFloat(mBody->GetLinearVelocity().y);
	pStream->writeFloat(mBody->GetPosition().x);
	pStream->writeFloat(mBody->GetPosition().y);
	pStream->writeFloat(direction);
	pStream->writeFloat(prevDir);
}

/**
* Deserializes player info from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Player::deserialize(PacketStream* pStream) {

	float xVel, yVel, xPos, yPos, dir, pDir;
	pStream->readFloat(xVel);
	pStream->readFloat(yVel);
	pStream->readFloat(xPos);
	pStream->readFloat(yPos);
	pStream->readFloat(dir);
	pStream->readFloat(pDir);

	//cout << "BEFORE: " << mBody->GetPosition().x << " " << mBody->GetPosition().y << endl;

	mBody->SetLinearVelocity(b2Vec2(xVel,yVel));
	mBody->SetTransform(b2Vec2(xPos, yPos), 0.0f);

	//cout << "AFTER: " << mBody->GetPosition().x << " " << mBody->GetPosition().y << endl;

	direction = dir;
	prevDir = pDir;
}

/**
* Saves the player to an XML file
* @param pDoc the file to save to
*/
void Player::save(TiXmlDocument& pDoc) {

	TiXmlElement * player = new TiXmlElement( "player" );
	pDoc.RootElement()->LinkEndChild( player );

	//float X = -((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)/32) + 258*4;		
	//(((-(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/64)*4)+(0.5*4))+(pPos.x*4)) + 0.5;

	string x = Constants::convertInt(mBody->GetPosition().x);

	TiXmlElement* xPos = new TiXmlElement( "xPos" );  
	xPos->LinkEndChild( new TiXmlText( x.c_str() ));
	player->LinkEndChild( xPos );

	string y = Constants::convertInt(mBody->GetPosition().y);

	TiXmlElement* yPos = new TiXmlElement( "yPos" );  
	yPos->LinkEndChild( new TiXmlText( y.c_str() ));
	player->LinkEndChild( yPos );
}

/**
* destructor
*/
Player::~Player() {

	//delete mImage;		mImage = NULL;
}