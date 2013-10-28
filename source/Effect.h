
#ifndef EFFECT_H
#define EFFECT_H

#include "Iw2D.h"

class Effect {

public:

	Effect(CIwSVec2 pPos, int pFrames, const char* pImageName, float pScale);
	~Effect();

	void update();
	bool isFinished();

private:

	void draw();

	CIwSVec2 mPos;
	CIw2DImage* mImage;
	int mFrameCount;
	int mCurFrame;
	int mFrameDelay;
	int mMaxFrameDelay;
	bool mFinished;
	float mScale;
};

#endif