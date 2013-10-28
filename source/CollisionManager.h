#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include "Box2D/Box2D.h"
#include "BombEnemy.h"
#include "Elevator.h"
#include "MsgSensor.h"
#include "player.h"
#include "math.h"
#include "LevelEnd.h"
#include "PacketStream.h"
#include "tinyxml.h"
#include "IwHTTP.h"
#include <string.h>
#include <sstream>
#include "Switch.h"
#include "HealthPickup.h"
#include "SpeedPickup.h"
#include "damagePickup.h"
#include "Crate.h"
#include "CrateSpawner.h"
#include "ActivatablePlatform.h"

class Game;
class MyContactListener : public b2ContactListener {

public:
	static int playersFinished;

	void setGame(Game* pGame);

private:
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);

	Game* mGame;
};
#include "game.h"

#endif