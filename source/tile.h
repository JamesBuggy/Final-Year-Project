#include "Iw2D.h"

#ifndef TILE_H
#define TILE_H

class Tile {

public:
	Tile(int pImgID, CIwSVec2 pPos);
	~Tile();

	int getImgID();
	CIwSVec2 getPosition();

private:

	int mImgID;
	CIwSVec2 mPos;

};

#endif