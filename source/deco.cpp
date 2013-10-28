#include "deco.h"

/**
* Constructor
* @param pImage the image to use for the deco
* @param pRes the resource file to use
* @param pPos the position of the decoration
* @param pSize the size of the decoration
*/
Deco::Deco(const char* pImage, const char* pRes, CIwSVec2 pPos, CIwSVec2 pSize) {

	mImage = Iw2DCreateImage(pImage);

	mPos = pPos;
	mSize = pSize;
}

/**
* Draws the deco image 
*/
void Deco::draw() {

	Iw2DDrawImage(mImage, mPos, mSize);
}

/**
* Destructor
*/
Deco::~Deco() {

	delete mImage;
	mImage = NULL;
}