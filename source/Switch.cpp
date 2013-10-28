#include "Switch.h"

/**
* Constructor
* @param pImgName the name of the image to use
* @param pPos the position of the pickup
* @param pDoor the door this switch opens
* @param pWorld the physics world to add the switch to
*/
Switch::Switch(const char* pImgName, CIwSVec2 pPos, Door* pDoor, b2World* pWorld):
	mDoor( pDoor ),
	mImage(Iw2DCreateImage(pImgName)),
	mPos(pPos),
	mImgOffset(0),
	mActive(false) {


		float X = -(768/16) + (1*4) + (pPos.x*4) + 0.5;
		float Y = (1024/16) - (1*4) + (-pPos.y*4) - 0.5;

		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(X, Y);
		mBody = pWorld->CreateBody(&bodyDef);

		b2PolygonShape polygonShape;
		polygonShape.SetAsBox((1 * (8/2)), (1 * (8/2)));
		b2FixtureDef fd;
		fd.shape = &polygonShape;
		fd.isSensor = true;
		mBody->CreateFixture(&fd);

		mToggleSoundSpec = (CIwSoundSpec*)IwGetResManager()->GetGroupNamed("Sounds")->GetResNamed("switch", IW_SOUND_RESTYPE_SPEC);
}

/**
* Draws the switch image
*/
void Switch::draw() {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

	b2Transform t = mBody->GetTransform();
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
	
	Iw2DDrawImageRegion(mImage, CIwSVec2(pos.x-32, pos.y-32), CIwSVec2(0 + mImgOffset, 0), CIwSVec2(mImage->GetWidth()/2, mImage->GetHeight()));
}

/**
* toggles the switch and opens the door
*/
void Switch::toggle() {

	if(mActive == false) {
		mActive = true;
		mImgOffset = 64;
		mDoor->toggle();
		mToggleSoundSpec->Play();
	}
}

/**
* Gets the position of the switch
* @return the position of the switch
*/
b2Vec2 Switch::getPosition() {
	return mBody->GetPosition();
}

/**
* destructor
*/
Switch::~Switch() {

}