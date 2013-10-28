#ifndef LEVEL_H
#define LEVEL_H

#include "Box2D/Box2D.h"
#include "tile.h"
#include "Iw2D.h"
#include <vector>
#include "math.h"
#include "spikeTrap.h"
#include "bladeTrap.h"
#include "MeleeEnemy.h"
#include "BombEnemy.h"
#include "Platform.h"
#include "Elevator.h"
#include "Checkpoint.h"
#include "MsgSensor.h"
#include "deco.h"
#include "IwUI.h"
#include "tinyxml.h"
#include "Constants.h"
#include "LevelEnd.h"
#include "Door.h"
#include "Switch.h"
#include "PacketStream.h"
#include "HealthPickup.h"
#include "SpeedPickup.h"
#include "DamagePickup.h"
#include "Crate.h"
#include "CrateSpawner.h"
#include "ActivatablePlatform.h"
#include "KillBox.h"

class Level {

public:
	Level(b2Vec2 pGravity, bool pSleep, CIwStringS pLevel, bool pLoading, int pDiff);
	~Level();

	void update(CIwSVec2 pPlayerPos, PacketStream* pStream, bool pHost);
	void drawLevel(CIwSVec2 pPlayerPos);
	void drawBackground(CIwSVec2 pPos, CIwSVec2 pTrans);
	void killEnemy(int pID);
	void destroyPickup(int pID);
	void createPickup(int pType, int pX, int pY);
	void destroyCrate(int pID);
	void createCrate(int pType, int pX, int pY);
	CIwStringS getNextLevel();
	CIwStringS getCurLevel();

	b2World* getWorld();
	vector<Enemy*> getEnemies();
	vector<Switch*> getSwitches();
	vector<Crate*> getCrates();
	vector<Pickup*> getPickups();
	vector<CrateSpawner*> getCrateSpawners();
	vector<ActivatablePlatform*> getActivatablePlatforms();

	void serialize(PacketStream* pStream);
	void deserialize(PacketStream* pStream);
	void save(TiXmlDocument& pDoc);

private:
	
	b2World* mWorld; // Box2D world

	std::vector<Tile*> mGeometry;
	std::vector<Trap*> mTraps;
	std::vector<Deco*> mDeco;
	std::vector<Platform*> mPlatforms;
	std::vector<Elevator*> mElevators;
	std::vector<Checkpoint*> mCheckpoints;
	std::vector<MSGSensor*> mMessages;
	std::vector<Door*> mDoors;
	std::vector<Switch*> mSwitches;
	std::vector<Pickup*> mPickups;
	std::vector<Crate*> mCrates;
	std::vector<CrateSpawner*> mCrateSpawners;
	std::vector<ActivatablePlatform*> mActivatablePlatforms;
	std::vector<KillBox*> mKillBoxs;
	LevelEnd* mEnding;
	CIwStringS mNextLevel;
	CIwStringS mCurLevel;

	CIw2DImage* mImg;
	CIw2DImage* terrainTopImg;
	CIw2DImage* terrainBtmImg;
	CIw2DImage* backGroundImg;

	void createGeometry(CIwStringS pLevel);
	void createB2dObject(float xPos, float yPos, float width, float height);
	void loadTrapsFromXml(TiXmlDocument pDoc);
	void loadEnemiesFromXml(TiXmlDocument pDoc, bool pLoading);
	void loadPlatformsFromXml(TiXmlDocument pDoc);
	void loadCheckpointsFromXml(TiXmlDocument pDoc);
	void loadMessagesFromXml(TiXmlDocument pDoc);
	void loadEndingFromXml(TiXmlDocument pDoc);
	void loadDoorsFromXml(TiXmlDocument pDoc);
	void loadPickupsFromXml(TiXmlDocument pDoc, bool pLoading);
	void loadCratesFromXml(TiXmlDocument pDoc, bool pLoading);
	void loadCrateSpanwersFromXml(TiXmlDocument pDoc);
	void loadKillboxesFromXml(TiXmlDocument pDoc);
	void loadActivatablePlatformsFromXml(TiXmlDocument pDoc);

	void drawGeometry(CIwSVec2 pPlayerPos);
	void updateTraps();
	void updateEnemies(CIwSVec2 pPlayerPos, PacketStream* pStream, bool pHost);
	void updatePlatforms();
	void updateElevators();
	void updateDoors();
	void updatePickups();
	void updateObjects();

	enum entityCat {
	
		PLAYER	= 0x0002,
		TRAP	= 0x0004,
	};

	vector<Enemy*> mEnemies;

	char* groundData;
	int mDiff;
};

#endif