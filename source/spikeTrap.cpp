#include "spikeTrap.h"

/**
* Constructor
* @param pImgName the image to use for the trap
* @param pPos the position of the trap
* @param pWorld the physics world the trap is to be added to
*/
SpikeTrap::SpikeTrap(const char* pImgName, CIwSVec2 pPos, b2World* pWorld): 
	Trap(pImgName, pPos) {

	//mImg = Iw2DCreateImage(pImgName);
	//mPos = CIwSVec2(pPos.x*32, (pPos.y*32)+4);

	float X = (((-(768/64)*4)+(0.5*4))+(pPos.x*4)) + 0.5;
	float Y = ((((1024/64)*4)-(0.5*4)) + (-pPos.y*4)) - 0.5;

	X = -(768/16) + (0.5*4) + (pPos.x*4) + 0.5;
	Y = (1024/16) - (0.5*4) - (pPos.y*4) - 0.5;

	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;
	bodyDef.position.Set(X, Y);

	mBody = pWorld->CreateBody(&bodyDef);
	mBody->SetLinearDamping(0.1f);
	mBody->SetFixedRotation(true);
	mType = "spikeTrap";

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(0.5f * (8/2), 0.2f * (8/2));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.userData = mType;
	fd.isSensor = true;
	fd.filter.categoryBits = Constants::TRAP;
	fd.filter.maskBits = Constants::PLAYER | Constants::ENEMY;

	mBody->CreateFixture(&fd);
	mBody->SetUserData(this);
}

/**
* Updates the trap
*/
void SpikeTrap::update() {
	
	draw();
}

/**
* draws the trap image
*/
void SpikeTrap::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

	Iw2DDrawImage(mImg, CIwSVec2(pos.x-16, pos.y-16), CIwSVec2(32, 32));
}

bool SpikeTrap::isColliding(CIwSVec2 pPoint) {

	/*for(int i = 0; i < pPoints.size(); i++) {
		if(pPoints.at(i).x >= mPos.x && pPoints.at(i).x <= mPos.x + mImg->GetWidth() &&
			pPoints.at(i).y >= mPos.y && pPoints.at(i).y <= mPos.y + mImg->GetHeight()) {

				return true;
		}
	}*/

	CIwSVec2 distance = (mPos + CIwSVec2(10/2, 10/2)) - pPoint;
	if( abs(distance.GetLength()) <= 80/2 ) {
						
		return true;
	}

	return false;
}

/**
* Destructor
*/
SpikeTrap::~SpikeTrap() {
	
}

