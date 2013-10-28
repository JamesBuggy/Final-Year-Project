#include "tile.h"

/**
* Constructor
* @param pImgID the id number of the image to use(used in level.cpp to decide what image to draw)
* @param pPos the position of the tile
*/
Tile::Tile(int pImgID, CIwSVec2 pPos) {

	mImgID = pImgID;

	mPos = pPos;
}

/**
* Gets the tiles image id
* @return the tile image id
*/
int Tile::getImgID() {

	return mImgID;
}

/**
* Gets the tiles position
* @return the tiles position
*/
CIwSVec2 Tile::getPosition() {

	return mPos;
}

/**
* Destructor
*/
Tile::~Tile() {


}