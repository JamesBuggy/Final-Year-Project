#include "bladeTrap.h"

/**
* Constructor
* @param pImage the image to use for the trap
* @param pPos the position of the trap
* @param pWorld the physics world the trap is to be added to
*/
BladeTrap::BladeTrap(const char* pImgName, CIwSVec2 pPos, b2World* pWorld): 
	Trap(pImgName, pPos),
	mFrame(0),
	mFrames(3) {


	float X = (((-(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/64)*4)+(0.5*4))+(pPos.x*4)) + 0.5;
	float Y = ((((s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/64)*4)-(0.5*4)) + (-pPos.y*4)) - 0.5;
	X = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5;
	Y = (1024/16) - (0.9*4) - (pPos.y*4) - 0.5;
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(X, Y);

	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetFixedRotation(true);

	mType = "spikeTrap";

	b2CircleShape shape;
	shape.m_radius = 4.3f;
	b2FixtureDef fd;
	fd.shape = &shape;
	fd.filter.categoryBits = Constants::TRAP;
	fd.filter.maskBits = Constants::ENEMY | Constants::PLAYER | Constants::OBJECT;
	fd.isSensor = true;
	fd.userData = mType;

	mBody->CreateFixture(&fd);
	mBody->SetUserData(this);
}

/**
* Trap update function
*/
void BladeTrap::update() {
	
	draw();
}

/**
* Draws and animates the trap
*/
void BladeTrap::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImageRegion(mImg, CIwSVec2(pos.x, pos.y)-CIwSVec2((mImg->GetWidth()/mFrames)/2, mImg->GetHeight()/2), CIwSVec2(mFrame*(mImg->GetWidth()/mFrames), 0), CIwSVec2(mImg->GetWidth()/mFrames, mImg->GetHeight()));

	mFrame++;

	if (mFrame >= mFrames) {
		mFrame = 0;
	}
}

bool BladeTrap::isColliding(CIwSVec2 pPoint) {

	/*CIwSVec2 center = CIwSVec2(mPos.x + (mImg->GetWidth()/2), mPos.y + (mImg->GetHeight()/2));

	for(int i = 0; i < pPoints.size(); i++) {
		CIwSVec2 distance = pPoints.at(i)-center;
		if( abs(distance.GetLength()) <= mImg->GetWidth()/2 ) {
			return true;
		}
	}*/

	CIwSVec2 distance = (mPos + CIwSVec2(74/2, 74/2)) - pPoint;
	if( abs(distance.GetLength()) <= 90/2 ) {
						
		return true;
	}

	return false;
}

/**
* Destructor
*/
BladeTrap::~BladeTrap() {
	
}

