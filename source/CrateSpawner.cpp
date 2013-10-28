#include "CrateSpawner.h"

/**
* Constructor
* @param pType the tyoe of crates to spawn
* @param pSpawnerPos the position of the spawner
* @param pSwitchPos the position of the switch
* @param pWorld the physics world to add the spawner to
*/
CrateSpawner::CrateSpawner(char* pType, b2Vec2 pSpawnerPos, b2Vec2 pSwitchPos, b2World* pWorld) {

	switchImgOffset = 0;
	mType = pType;

	float X;
	float Y;

	X = (-(768/64)*4) + (1.5*4) + (pSpawnerPos.x*4) + 0.5;
	Y = ((1024/64)*4)-(1.0*4) + (-pSpawnerPos.y*4) - 0.5;

	b2BodyDef spawnerBodyDef;
	spawnerBodyDef.type = b2_staticBody;
	spawnerBodyDef.position.Set(X, Y);
	mSpawnerBody = pWorld->CreateBody(&spawnerBodyDef);
	mSpawnerBody->SetFixedRotation(true);

	b2PolygonShape spawnerShape;
	spawnerShape.SetAsBox((1.5f * (8/2)), (1.0f * (8/2)));

	b2FixtureDef spawnerFd;
	spawnerFd.shape = &spawnerShape;
	spawnerFd.friction = 0.5f;
	spawnerFd.density = 5.0f;
	spawnerFd.restitution = 0.0f;
	spawnerFd.filter.categoryBits = Constants::OBJECT;
	spawnerFd.filter.maskBits = Constants::PLAYER | Constants::ENEMY;

	mSpawnerBody->CreateFixture(&spawnerFd);
	if(pType == "metalCrateSpawner") {
		mSpawnerImg = Iw2DCreateImage("Images/woodenCrateSpawner.png");
	}
	else if(pType == "woodenCrateSpawner") {
		mSpawnerImg = Iw2DCreateImage("Images/woodenCrateSpawner.png");
	}

	X = (-(768/64)*4) + (1.5*4) + (pSwitchPos.x*4) + 0.5;
	Y = ((1024/64)*4)-(1.0*4) + (-pSwitchPos.y*4) - 0.5;

	b2BodyDef switchBodyDef;
	switchBodyDef.type = b2_staticBody;
	switchBodyDef.position.Set(X, Y);
	mSwitchBody = pWorld->CreateBody(&switchBodyDef);
	mSwitchBody->SetFixedRotation(true);

	b2PolygonShape switchShape;
	switchShape.SetAsBox((1.0f * (8/2)), (1.0f * (8/2)));

	b2FixtureDef switchFd;
	switchFd.shape = &switchShape;
	switchFd.isSensor = true;

	mSwitchBody->CreateFixture(&switchFd);
	mSwitchImg = Iw2DCreateImage("Images/switch.png");

	mSwitchBody->SetUserData(this);
}

/**
* Draws the spawner and switch
*/
void CrateSpawner::draw() {

	// Draw spawner image
	CIwSVec2 imageSize(mSpawnerImg->GetWidth() >> 3, mSpawnerImg->GetHeight() >> 3);
	CIwSVec2 halfImageSize = CIwSVec2(mSpawnerImg->GetWidth()/2, mSpawnerImg->GetHeight()/2);
	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
	b2Transform t = mSpawnerBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
	Iw2DDrawImage(mSpawnerImg, pos - halfImageSize, CIwSVec2(mSpawnerImg->GetWidth(), mSpawnerImg->GetHeight()));

	// Draw switch image
	imageSize = CIwSVec2(mSwitchImg->GetWidth() >> 3, mSwitchImg->GetHeight() >> 3);
	halfImageSize = CIwSVec2(mSwitchImg->GetWidth()/2, mSwitchImg->GetHeight()/2);
	screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
	t = mSwitchBody->GetTransform();
	pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
	Iw2DDrawImageRegion(mSwitchImg, pos - halfImageSize, CIwSVec2(0 + switchImgOffset, 0), CIwSVec2(mSwitchImg->GetWidth()/2, mSwitchImg->GetHeight()));
}

/**
* Gets the physics body of the spawner
* @return the spawners physics body
*/
b2Body* CrateSpawner::getSpawner() {
	return mSpawnerBody;
}

/**
* Gets the physics body of the switch
* @return the switches physics body
*/
b2Body* CrateSpawner::getSwitch() {
	return mSwitchBody;
}

/**
* Gets the spawners type
* @return the type of the spawner
*/
char* CrateSpawner::getType() {
	return mType;
}

/**
* toggles the spawners switch
*/
void CrateSpawner::toggleSwitch() {

	if(switchImgOffset == 0) {
		switchImgOffset = 64;
	}
	else {
		switchImgOffset = 0;
	}
}

/**
* Destructor
*/
CrateSpawner::~CrateSpawner() {

}