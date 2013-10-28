#include "Effect.h"

/**
* Constructor
* @param pPos the position of the door
* @param pFrames number of frames in the animation
* @param pImageName the image to use for the animation
* @param pScale the render scale
*/
Effect::Effect(CIwSVec2 pPos, int pFrames, const char* pImageName, float pScale):
	mPos(pPos),
	mFrameCount(pFrames),
	mCurFrame(0),
	mFrameDelay(0),
	mMaxFrameDelay(3),
	mFinished(false),
	mScale(pScale) {

		CIwImage img;
		img.LoadFromFile(pImageName);
		mImage = Iw2DCreateImage(img);
}

/**
* Effect update function
*/
void Effect::update() {

	if(!mFinished) {
		
		if(mFrameDelay <= 0) {
			mCurFrame++;
			mFrameDelay = mMaxFrameDelay;

			if(mCurFrame >= mFrameCount) {
				mFinished = true;
			}
		}

		if(mFrameDelay > 0) {
			mFrameDelay--;
		}

		if(!mFinished) {
			this->draw();
		}
	}
}

/**
* Draws and animates the effect
*/
void Effect::draw() {

	CIwSVec2 pos = CIwSVec2(mPos.x-((mImage->GetWidth()/mFrameCount)/2)*mScale, mPos.y-(mImage->GetHeight()/2)*mScale);

	Iw2DDrawImageRegion(mImage, pos, CIwSVec2((mImage->GetWidth()/mFrameCount)*mScale, mImage->GetHeight()), CIwSVec2(mCurFrame*(mImage->GetWidth()/mFrameCount), 0), CIwSVec2(mImage->GetWidth()/mFrameCount, mImage->GetHeight()) );
}

/**
* gets the state of the effect
* @return the state of the effect
*/
bool Effect::isFinished() {

	return mFinished;
}

/**
* Destructor
*/
Effect::~Effect() {

}