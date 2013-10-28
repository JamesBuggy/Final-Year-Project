/**
* Contains static constants for the game
*/

#include <string.h>
#include <sstream>
#include "Box2D\Box2D.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

static float DIFFICULTYSCALE = 1;

class Constants {

public:
	
	// Physics collison filtering values
	static const int PLAYER = 0x0001;
	static const int ENEMY = 0x0002;
	static const int TERRAIN = 0x0004;
	static const int PLATFORM = 0x0008;
	static const int TRAP = 0x0010;
	static const int SENSOR = 0x0012;
	static const int PICKUP = 0x0014;
	static const int OBJECT = 0x0016;
	static const int KILLBOX = 0x0018;

	enum s3eFBStatus
	{
		FB_UNINITIALISED,
		FB_LOGGED_OUT,
		FB_LOGGING_IN,
		FB_LOGGED_IN
	};

	enum Difficulty{
	
		EASY = 1,
		NORMAL = 2,
		HARD = 3
	};

	enum GameStage {
		LOBBY = 1,
		INGAME = 2,
		LEVELSELECT = 3,
		MAINMENU = 4,
		ENDLEVEL = 5
	};

	// Multiplayer message types
	enum mMessageType{
	
		UPDATE = 1,
		DRUPDATE = 2,
		CREATE = 3,
		ACTION = 4,
		DESTROY = 5,
		PLAYERFOUND = 6,
		LOADLEVEL = 7,
		SELECTLEVEL = 8,
		READY = 9,
		STARTGAME = 10,
		CREATECRATE = 11,
		CREATEPICKUP = 12,
		DESTROYCRATE = 13,
		DESTROYPICKUP = 14,
		TOGGLEPLATFORM = 15,
		SWITCH = 16
	};

	static std::string convertInt(int pNum) {

	   std::stringstream ss;
	   ss << pNum;
	   return ss.str();
	}
};

#endif