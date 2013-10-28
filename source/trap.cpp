#include "trap.h"

/**
* Constructor
* @param pImgName the name of the image to use
* @param pPos the position of the trap
*/
Trap::Trap(const char* pImgName, CIwSVec2 pPos) {

	CIwImage img;
	img.LoadFromFile(pImgName);
	mImg = Iw2DCreateImage(img);
	mPos = CIwSVec2(pPos.x*32, (pPos.y*32)+4);	
}

/**
* Destructor
*/
Trap::~Trap() {
	delete mImg;		mImg = NULL;
}