#include "level.h"

/**
* Constructor
* @param pGravity the gravity vector
* @param pSleep if bodies should sleep or not
* @param pLevel the name of the level
* @param pLoading is the level loaded from a file
* @param pDiff the difficulty of the game
*/
Level::Level(b2Vec2 pGravity, bool pSleep, CIwStringS pLevel, bool pLoading, int pDiff):
	mDiff(pDiff),
	mWorld( new b2World(pGravity, pSleep) ) {

	CIwStringS dataFile = "Levels/" + pLevel + "Data.xml";
	
	int xPos, yPos, type;
	TiXmlDocument doc;
	doc.LoadFile(dataFile.c_str());
	s3eDeviceYield(100);
	loadTrapsFromXml(doc);
	loadPlatformsFromXml(doc);
	loadCheckpointsFromXml(doc);
	loadEndingFromXml(doc);
	loadMessagesFromXml(doc);
	loadDoorsFromXml(doc);
	loadCrateSpanwersFromXml(doc);
	loadKillboxesFromXml(doc);
	loadActivatablePlatformsFromXml(doc);

	if(pLoading) {

		TiXmlDocument loadingDoc;
		loadingDoc.LoadFile("save.xml");
		loadEnemiesFromXml(loadingDoc, pLoading);
		loadPickupsFromXml(loadingDoc, pLoading);
		loadCratesFromXml(loadingDoc, pLoading);
	}
	else {
		loadEnemiesFromXml(doc, pLoading);
		loadPickupsFromXml(doc, pLoading);
		loadCratesFromXml(doc, pLoading);
	}

	TiXmlElement * root = doc.RootElement();
	TiXmlElement * endingRoot = root->FirstChildElement("nextLevel");
	TiXmlElement * info;
	info = endingRoot->FirstChildElement("name");
	mNextLevel = info->FirstChild()->Value();
	
	endingRoot = root->FirstChildElement("thisLevel");
	info = endingRoot->FirstChildElement("name");
	mCurLevel = info->FirstChild()->Value();

	createGeometry(pLevel);

	CIwImage img;
	img.LoadFromFile("Images/topTile.png");
	terrainTopImg = Iw2DCreateImage(img);
	CIwImage img2;
	img2.LoadFromFile("Images/tile.png");
	terrainBtmImg = Iw2DCreateImage(img2);
	CIwImage img3;
	img3.LoadFromFile("Images/background.bmp");
	backGroundImg = Iw2DCreateImage(img3);

	/*mCrates.push_back(new Crate("woodenCrate", CIwSVec2(27, 27), mWorld));

	mCrateSpawners.push_back(new CrateSpawner("woodenCrateSpawner", b2Vec2(26, 21), b2Vec2(23, 27), mWorld));

	mActivatablePlatforms.push_back(new ActivatablePlatform("Images/platform.png", CIwSVec2(66, 39), CIwSVec2(66, 37), 2, 50, 59, mWorld));

	mKillBoxs.push_back(new KillBox(b2Vec2(31, 44), b2Vec2(1.5, 1), mWorld));*/
}

/**
* Updates the level and all objects within
* @param pPlayerPos the position of the player
* @param pStream the packstream to write object data to in multiplayer
* @param pHost if this game is the host or not
*/
void Level::update(CIwSVec2 pPlayerPos, PacketStream* pStream, bool pHost) {
	
	updateEnemies(pPlayerPos, pStream, pHost);
	updatePlatforms();
	updateElevators();
	updateTraps();
	updateDoors();
	updatePickups();
	updateObjects();
	mEnding->draw();

	for(int i = 0; i < mSwitches.size(); i++) {
	
		mSwitches.at(i)->draw();
	}
	for(int i = 0; i < mCrateSpawners.size(); i++) {
	
		mCrateSpawners.at(i)->draw();
	}
	for(int i = 0; i < mActivatablePlatforms.size(); i++) {
	
		mActivatablePlatforms.at(i)->update();
	}

	drawGeometry(pPlayerPos);
}

/**
* Draws all level objects
* @param pPlayerPos the position of the player
*/
void Level::drawLevel(CIwSVec2 pPlayerPos) {

	for(int i = 0; i < mEnemies.size(); ++i) {
	
		if(mEnemies.at(i)->isAlive()) {
		
			mEnemies.at(i)->draw();
		}
	}
	for(int i = 0; i < mPlatforms.size(); ++i) {
	
		mPlatforms.at(i)->draw();
	}
	for(int i = 0; i < mElevators.size(); ++i) {
	
		mElevators.at(i)->draw();
	}
	for(int i = 0; i < mTraps.size(); ++i) {
		mTraps.at(i)->draw();
	}
	
	drawGeometry(pPlayerPos);
}

/**
* Draws the background image
* @param pPos the position to draw the background
* @param pTrans the translation(would be used for paralax scrolling)
*/
void Level::drawBackground(CIwSVec2 pPos, CIwSVec2 pTrans) {

	Iw2DDrawImageRegion(backGroundImg, CIwSVec2(0, 0), CIwSVec2(1024, 768), CIwSVec2(0, 0), CIwSVec2(backGroundImg->GetWidth(), backGroundImg->GetHeight()));
}

/**
* Draws the level geometry
* @param pPlayerPos the player position (used to optimize the geometry drawing)
*/
void Level::drawGeometry(CIwSVec2 pPlayerPos) {

	CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
	CIwSVec2 pos = screenCentre + (CIwSVec2(int16(pPlayerPos.x*8), -int16(pPlayerPos.y*8)));

	for(int i = 0; i < mDeco.size(); i++) {
		mDeco.at(i)->draw();
	}

	static const CIwSVec2 imageSize(44, 44);
	for(int i =0; i < mGeometry.size(); i++) {

		CIwSVec2 terPos = mGeometry.at(i)->getPosition();

		if(terPos.x >= (pos.x)-(1024/2)-32 && terPos.x <= (pos.x)+(1024/2) &&
			terPos.y >= (pos.y)-(768/2)-32 && terPos.y <= (pos.y)+(768/2)) {

				if(mGeometry.at(i)->getImgID() == 1) {
					Iw2DDrawImage(terrainTopImg, CIwSVec2(mGeometry.at(i)->getPosition().x-3, mGeometry.at(i)->getPosition().y-3), imageSize);
				}
				else {
					Iw2DDrawImage(terrainBtmImg, CIwSVec2(mGeometry.at(i)->getPosition().x-3, mGeometry.at(i)->getPosition().y-3), imageSize);
				}
		}
	}
}

/**
* Gets the next level
* @return the name of the next level
*/
CIwStringS Level::getNextLevel() {

	return mNextLevel;
}

/**
* Updates objects in the level
*/
void Level::updateObjects() {

	for(int i = 0; i < mCrates.size(); ++i) {

		if(mCrates.at(i)->isAlive()) {
		
			mCrates.at(i)->update();
		}
		else {
			
			mWorld->DestroyBody(mCrates.at(i)->getBody());
			delete mCrates.at(i);
			mCrates.erase(mCrates.begin() + i);
		}
	}
}

/**
* Updates traps in the level
*/
void Level::updateTraps() {

	for(int i = 0; i < mTraps.size(); ++i) {
	
		mTraps.at(i)->update();
	}
}

/**
* Updates enemies in the level
* @param pPlayerPos the position of the player
* @param pStream the packet stream to write enemy data to write data to in multiplayer
* @param pHost if this game is host or not
*/
void Level::updateEnemies(CIwSVec2 pPlayerPos, PacketStream* pStream, bool pHost) {

	for(int i = 0; i < mEnemies.size(); ++i) {
	
		if(mEnemies.at(i)->isAlive()) {
		
			if(mEnemies.at(i)->isAttacking()) {
				mEnemies.at(i)->setTarget(b2Vec2(pPlayerPos.x, pPlayerPos.y));
			}
			mEnemies.at(i)->update(pHost);
		}
		else {

			if(pHost) {

				pStream->writeInt(Constants::DESTROY);// Destroy message
				pStream->writeInt(mEnemies.at(i)->getID());
			}

			mWorld->DestroyBody(mEnemies.at(i)->getBody());
			delete mEnemies.at(i);
			mEnemies.erase(mEnemies.begin() + i);
		}
	}
}

/**
* Updates pickups in the level
*/
void Level::updatePickups() {

	for(int i = 0; i < mPickups.size(); ++i) {
		mPickups.at(i)->update();

		if(mPickups.at(i)->isPickedUp() == true) {

			mPickups.at(i)->destroy(mWorld);
			delete mPickups.at(i);
			mPickups.erase(mPickups.begin() + i);
		}
	}
}

/**
* Updates platforms in the level
*/
void Level::updatePlatforms() {

	for(int i = 0; i < mPlatforms.size(); ++i) {
	
		mPlatforms.at(i)->update();
	}
}

/**
* Kills an enemy
* @param pID the id number of the enemy to kill
*/
void Level::killEnemy(int pID) {

	for(int i = 0; i < mEnemies.size(); i++) {

		if(mEnemies.at(i)->getID() == pID) {

			mEnemies.at(i)->kill();
		}
	}
}

/**
* Destroys pickups
* @param pID the id number of the pickups
*/
void Level::destroyPickup(int pID) {

	for(int i = 0; i < mPickups.size(); i++) {

		if(mPickups.at(i)->getID() == pID) {

			mPickups.at(i)->pickUp();
		}
	}
}

/**
* Creates a crate
* @param pType the type of crate to create
* @param pX the xPos at which to create the crate
* @param pY the yPos at which to create the crate
*/
void Level::createCrate(int pType, int pX, int pY) {

	if(pType == 1) {
		mCrates.push_back(new Crate("woodenCrate", CIwSVec2(pX, pY), mWorld, false));
	}
	else if(pType == 2) {
		mCrates.push_back(new Crate("metalCrate", CIwSVec2(pX, pY), mWorld, false));
	}
}

/**
* Destroys a crate
* @param pID the id number of the crate to destroy
*/
void Level::destroyCrate(int pID) {

	for(int i = 0; i < mCrates.size(); i++) {

		if(mCrates.at(i)->getID() == pID) {

			//mPickups.at(i)->pickUp();
			mWorld->DestroyBody(mCrates.at(i)->getBody());
			delete mCrates.at(i);
			mCrates.erase(mCrates.begin() + i);
		}
	}
}

/**
* Creates a pickup
* @param pType the type of crate to pickup
* @param pX the xPos at which to create the pickup
* @param pY the yPos at which to create the pickup
*/
void Level::createPickup(int pType, int pX, int pY) {

	if(pType == 0) { // Health pickup
		mPickups.push_back(new HealthPickup(CIwSVec2(pX, pY), mWorld, true, true));
	}
	else if(pType == 1) { // Speed pickup
		mPickups.push_back(new SpeedPickup(CIwSVec2(pX, pY), mWorld, true, true));
	}
	else if(pType == 2) { // Damage pickup
		mPickups.push_back(new DamagePickup(CIwSVec2(pX, pY), mWorld, true, true));
	}	
}

/**
* Updates elevators in the level
*/
void Level::updateElevators() {

	for(int i = 0; i < mElevators.size(); ++i) {
	
		mElevators.at(i)->update();
	}
}

/**
* Updates Doors in the level
*/
void Level::updateDoors() {

	for(int i = 0; i < mDoors.size(); ++i) {
	
		mDoors.at(i)->update();
	}
}

/**
* Gets the list of enemies
* @return the list of enemies
*/
vector<Enemy*> Level::getEnemies() {

	return mEnemies;
}

/**
* Gets the list of switches
* @return the list of switches
*/
vector<Switch*> Level::getSwitches() {
	return mSwitches;
}

/**
* Gets the list of crates
* @return the list of crates
*/
vector<Crate*> Level::getCrates() {
	return mCrates;
}

/**
* Gets the list of pickups
* @return the list of pickups
*/
vector<Pickup*> Level::getPickups() {
	return mPickups;
}

/**
* Gets the list of spawners
* @return the list of spawners
*/
vector<CrateSpawner*> Level::getCrateSpawners() {
	return mCrateSpawners;
}

/**
* Gets the list of activatable platforms
* @return the list of activatable platforms
*/
vector<ActivatablePlatform*> Level::getActivatablePlatforms() {

	return mActivatablePlatforms;
}

/**
* Creates a physics object to act as terrain
* @param xPos the xpos of the body
* @param yPos the yPos of the body
* @param width the width of the body
* @param height the height of the body
*/
void Level::createB2dObject(float xPos, float yPos, float width, float height) {
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_staticBody;

	float X = -(768/16) +(width*4)+ (xPos*4)+0.5;
	float Y = (1024/16) -(height*4)+ (yPos*4)-0.5;

	bodyDef.position.Set(X, Y);

	b2Body* groundBody;
	groundBody = mWorld->CreateBody(&bodyDef);
	groundBody->SetLinearDamping(0.1f);

	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(width * (8/2), height * (8/2));

	b2FixtureDef fd;
	fd.shape = &polygonShape;
	fd.friction = 10.0f;
	fd.density = 5.0f;
	fd.restitution = 0.0f;
	fd.filter.categoryBits = Constants::TERRAIN;
	fd.filter.maskBits = Constants::ENEMY | Constants::PLAYER | Constants::PICKUP;
	groundData = "ground";
	fd.userData = groundData;

	groundBody->CreateFixture(&fd);
}

/**
* Loads traps from a file
* @param pDoc the file to load from
*/
void Level::loadTrapsFromXml(TiXmlDocument pDoc) {

	int xPos, yPos, width;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * trapRoot = root->FirstChildElement("spikeTrap");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("spikeTrapNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {
		info = trapRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = trapRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = trapRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());

		for(int j = 0; j < width; j++) {
			mTraps.push_back(new SpikeTrap("Images/spikes.png", CIwSVec2(xPos+j, yPos), mWorld));
		}

		trapRoot = trapRoot->NextSiblingElement();
	}

	trapRoot = root->FirstChildElement("bladeTrap");
	count = atof(root->FirstChildElement("bladeTrapNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {
		info = trapRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = trapRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());

		mTraps.push_back(new BladeTrap("Images/bladeAnim.png", CIwSVec2(xPos, yPos), mWorld));

		trapRoot = trapRoot->NextSiblingElement();
	}
}

/**
* Loads traps from a enemies
* @param pDoc the file to load from
*/
void Level::loadEnemiesFromXml(TiXmlDocument pDoc, bool pLoading) {

	int xPos, yPos, type, patA, patB;
	int id = 0;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * enemyRoot = root->FirstChildElement("enemy");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("enemyNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {
		
		info = enemyRoot->FirstChildElement("type");
		type = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("patrolA");
		patA = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("patrolB");
		patB = atof(info->FirstChild()->Value());

		//BombEnemy
		if(type == 1) {
			mEnemies.push_back(new BombEnemy(mWorld, CIwSVec2(xPos, yPos), id, pLoading, mDiff, patA, patB));
		}
		//MeleeEnemy
		else if(type == 2) {
			mEnemies.push_back(new MeleeEnemy(mWorld, CIwSVec2(xPos, yPos), id, pLoading, mDiff, patA, patB));
		}

		enemyRoot = enemyRoot->NextSiblingElement();
		id++;
	}
}

/**
* Loads traps from a platforms
* @param pDoc the file to load from
*/
void Level::loadPlatformsFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, ext1, ext2;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * platRoot = root->FirstChildElement("platform");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("platformNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {
		info = platRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("extent1");
		ext1 = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("extent2");
		ext2 = atof(info->FirstChild()->Value());
	
		mPlatforms.push_back( new Platform("Images/platform.png", CIwSVec2(xPos, yPos), width, ext1, ext2, mWorld));

		platRoot = platRoot->NextSiblingElement();
	}

	platRoot = root->FirstChildElement("elevator");
	count = atof(root->FirstChildElement("elevatorNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {
		info = platRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("extent1");
		ext1 = atof(info->FirstChild()->Value());
		info = platRoot->FirstChildElement("extent2");
		ext2 = atof(info->FirstChild()->Value());
	
		mElevators.push_back( new Elevator("Images/platform.png", CIwSVec2(xPos, yPos), width, ext1, ext2, mWorld));

		platRoot = platRoot->NextSiblingElement();
	}
}

/**
* Loads checkpoints from a file
* @param pDoc the file to load from
*/
void Level::loadCheckpointsFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, height;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * checkpointRoot = root->FirstChildElement("checkpoint");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("checkpointNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = checkpointRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = checkpointRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = checkpointRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = checkpointRoot->FirstChildElement("height");
		height = atof(info->FirstChild()->Value());

		mCheckpoints.push_back(new Checkpoint(CIwSVec2(xPos, yPos), CIwSVec2(width, height), mWorld));

		checkpointRoot = checkpointRoot->NextSiblingElement();
	}
}

/**
* Loads levelEnding from a file
* @param pDoc the file to load from
*/
void Level::loadEndingFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, height;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * endingRoot = root->FirstChildElement("levelEnd");
	TiXmlElement * info;

	info = endingRoot->FirstChildElement("xPos");
	xPos = atof(info->FirstChild()->Value());
	info = endingRoot->FirstChildElement("yPos");
	yPos = atof(info->FirstChild()->Value());
	info = endingRoot->FirstChildElement("width");
	width = atof(info->FirstChild()->Value());
	info = endingRoot->FirstChildElement("height");
	height = atof(info->FirstChild()->Value());

	mEnding = new LevelEnd(b2Vec2(xPos, yPos), b2Vec2(width, height), "Images/portal.png", mWorld);
}

/**
* Loads tutorial messages from a file
* @param pDoc the file to load from
*/
void Level::loadMessagesFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, height;
	CIwStringS msgID;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * enemyRoot = root->FirstChildElement("message");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("msgNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = enemyRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("height");
		height = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("msgID");
		msgID = info->FirstChild()->Value();

		mMessages.push_back(new MSGSensor(b2Vec2(xPos, yPos), b2Vec2(width, height), msgID, mWorld));
		
		enemyRoot = enemyRoot->NextSiblingElement();
	}
}

/**
* Loads doors from a file
* @param pDoc the file to load from
*/
void Level::loadDoorsFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, ext, dir, switchX, switchY;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * enemyRoot = root->FirstChildElement("door");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("doorNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = enemyRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("extent");
		ext = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("direction");
		dir = atof(info->FirstChild()->Value());

		info = enemyRoot->FirstChildElement("switchX");
		switchX = atof(info->FirstChild()->Value());
		info = enemyRoot->FirstChildElement("switchY");
		switchY = atof(info->FirstChild()->Value());

		mDoors.push_back( new Door("Images/door.png", CIwSVec2(xPos, yPos), width, ext, dir, mWorld) );
		mSwitches.push_back( new Switch("Images/switch.png", CIwSVec2(switchX, switchY), mDoors.back(), mWorld) );
		
		enemyRoot = enemyRoot->NextSiblingElement();
	}
}

/**
* Loads pickups from a file
* @param pDoc the file to load from
*/
void Level::loadPickupsFromXml(TiXmlDocument pDoc, bool pLoading) {

	int xPos, yPos, type;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * pickupRoot = root->FirstChildElement("pickup");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("pickupNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = pickupRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = pickupRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = pickupRoot->FirstChildElement("type");
		type = atof(info->FirstChild()->Value());

		if(type == 0) { // Health pickup
			mPickups.push_back(new HealthPickup(CIwSVec2(xPos, yPos), mWorld, false, pLoading));
		}
		else if(type == 1) { // Speed pickup
			mPickups.push_back(new SpeedPickup(CIwSVec2(xPos, yPos), mWorld, false, pLoading));
		}
		else if(type == 2) { // Damage pickup
			mPickups.push_back(new DamagePickup(CIwSVec2(xPos, yPos), mWorld, false, pLoading));
		}
	
		pickupRoot = pickupRoot->NextSiblingElement();
	}
}

/**
* Loads crates from a file
* @param pDoc the file to load from
*/
void Level::loadCratesFromXml(TiXmlDocument pDoc, bool pLoading) {

	float xPos, yPos;
	int type;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * crateRoot = root->FirstChildElement("crate");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("crateNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = crateRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("type");
		type = atoi(info->FirstChild()->Value());

		if(type == 0) {
			mCrates.push_back(new Crate("woodenCrate", CIwSVec2(xPos, yPos), mWorld, pLoading));
		}
		else if(type == 1) {
			mCrates.push_back(new Crate("metalCrate", CIwSVec2(xPos, yPos), mWorld, pLoading));
		}

		crateRoot = crateRoot->NextSiblingElement();
	}
}

/**
* Loads crate spawners from a file
* @param pDoc the file to load from
*/
void Level::loadCrateSpanwersFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, switchX, switchY;
	int type;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * crateRoot = root->FirstChildElement("crateSpawner");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("crateSpawnerNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = crateRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("type");
		type = atoi(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("switchX");
		switchX = atof(info->FirstChild()->Value());
		info = crateRoot->FirstChildElement("switchY");
		switchY = atof(info->FirstChild()->Value());

		if(type == 0) {
			mCrateSpawners.push_back(new CrateSpawner("woodenCrateSpawner", b2Vec2(xPos, yPos), b2Vec2(switchX, switchY), mWorld));
		}
		else if(type == 1) {
			mCrateSpawners.push_back(new CrateSpawner("metalCrateSpawner", b2Vec2(xPos, yPos), b2Vec2(switchX, switchY), mWorld));
		}
		
		crateRoot = crateRoot->NextSiblingElement();
	}
}

/**
* Loads kill boxes from a file
* @param pDoc the file to load from
*/
void Level::loadKillboxesFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, height;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * killboxRoot = root->FirstChildElement("killbox");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("killboxNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = killboxRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = killboxRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = killboxRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = killboxRoot->FirstChildElement("height");
		height = atof(info->FirstChild()->Value());

		mKillBoxs.push_back(new KillBox(b2Vec2(xPos, yPos), b2Vec2(width, height), mWorld));
		
		killboxRoot = killboxRoot->NextSiblingElement();
	}
}

/**
* Loads activatable platforms from a file
* @param pDoc the file to load from
*/
void Level::loadActivatablePlatformsFromXml(TiXmlDocument pDoc) {

	float xPos, yPos, width, switchX, switchY, ext1, ext2;

	TiXmlElement * root = pDoc.RootElement();
	TiXmlElement * APRoot = root->FirstChildElement("activatablePlatform");
	TiXmlElement * info;

	int count = atof(root->FirstChildElement("activatablePlatformNum")->FirstChild()->Value());

	for(int i = 0; i < count; i++) {

		info = APRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("width");
		width = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("switchX");
		switchX = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("switchY");
		switchY = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("extent1");
		ext1 = atof(info->FirstChild()->Value());
		info = APRoot->FirstChildElement("extent2");
		ext2 = atof(info->FirstChild()->Value());

		mActivatablePlatforms.push_back(new ActivatablePlatform("Images/platform.png", CIwSVec2(xPos, yPos), CIwSVec2(switchX, switchY), width, ext1, ext2, mWorld));
		
		APRoot = APRoot->NextSiblingElement();
	}
}

/**
* Creates the level geometry
* @param pLevel the name of the level
*/
void Level::createGeometry(CIwStringS pLevel) {

	CIwStringS levelImg = "Levels/" + pLevel + ".bmp";

	CIwImage img;
	img.LoadFromFile(levelImg.c_str());
	uint8* test = img.GetTexels();

	int level[80][80];

	int j = 0;// Vertical offset
	int k = 0;
	for(int i = 0; i < img.GetWidth()*img.GetHeight(); i++) {
		
		level[k][j] = test[i];

		k++;
		if(k == img.GetWidth()) {
			k = 0;
			j++;
		}
	}

	// Create terrain physics objects
	for(int x = 0; x < 80; x++) {
		for(int y = 0; y < 80; y++) {
			
			if(level[x][y] == 0) {

				createB2dObject(x, -y, 0.5f, 0.5f);

				float X = -(768/16) +(0.5*4)+ (x*4)+0.5;
				float Y = (1024/16) -(0.5*4)- (y*4)-0.5;

				CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

				CIwSVec2 pos = screenCentre + (CIwSVec2(int16(X*8), -int16(Y*8)));

				if(level[x][y-1] != 0) {
					
						mGeometry.push_back(new Tile(1, CIwSVec2(pos.x-16, pos.y-16)));
				}
				else { 
					
						mGeometry.push_back(new Tile(2, CIwSVec2(pos.x-16, pos.y-16)));
				}

			}
		}
	}
}

/**
* Gets the physics world
* @return the physics world
*/
b2World* Level::getWorld() {
	return mWorld;
}

/**
* Gets the current level name
* @return thename of the current level
*/
CIwStringS Level::getCurLevel() {

	return mCurLevel;
}

/**
* Serializes the level to be sent across a network during multiplayer
* @param pStream The packet stream to write the info to
*/
void Level::serialize(PacketStream* pStream) {

	for(int i = 0; i < mPlatforms.size(); i++) {
		mPlatforms.at(i)->serialize(pStream);
	}
	for(int i = 0; i < mElevators.size(); i++) {
		mElevators.at(i)->serialize(pStream);
	}
	for(int i = 0; i < mEnemies.size(); i++) {
		mEnemies.at(i)->serialize(pStream);
	}
	for(int i = 0; i < mDoors.size(); i++) {
		mDoors.at(i)->serialize(pStream);
	}
	for(int i = 0; i < mActivatablePlatforms.size(); i++) {
		mActivatablePlatforms.at(i)->serialize(pStream);
	}
	for(int i = 0; i < mCrates.size(); i++) {
		mCrates.at(i)->serialize(pStream);
	}
	/*for(int i = 0; i < mPickups.size(); i++) {
		mPickups.at(i)->serialize(pStream);
	}*/
}

/**
* Deserializes the level from a packet stream during multiplayer
* @param pStream the packet stream to read from
*/
void Level::deserialize(PacketStream* pStream) {

	for(int i = 0; i < mPlatforms.size(); i++) {
		mPlatforms.at(i)->deserialize(pStream);
	}
	for(int i = 0; i < mElevators.size(); i++) {
		mElevators.at(i)->deserialize(pStream);
	}
	for(int i = 0; i < mEnemies.size(); i++) {
		mEnemies.at(i)->deserialize(pStream);
	}
	for(int i = 0; i < mDoors.size(); i++) {
		mDoors.at(i)->deserialize(pStream);
	}
	for(int i = 0; i < mActivatablePlatforms.size(); i++) {
		mActivatablePlatforms.at(i)->deserialize(pStream);
	}
	for(int i = 0; i < mCrates.size(); i++) {

		//int id;
		//pStream->readInt(id);

		//for(int i = 0; i < mCrates.size(); i++) {
			//if(mCrates.at(i)->getID() == id) {
				mCrates.at(i)->deserialize(pStream);
			//}
		//}
	}
	/*for(int i = 0; i < mPickups.size(); i++) {
		mPickups.at(i)->deserialize(pStream);
	}*/
}

/**
* Saves the level to an XML file
* @param pDoc the file to save to
*/
void Level::save(TiXmlDocument& pDoc) {

	TiXmlElement * enemyNum = new TiXmlElement( "enemyNum" ); 
	enemyNum->LinkEndChild( new TiXmlText( Constants::convertInt(mEnemies.size()).c_str() ));
	pDoc.RootElement()->LinkEndChild( enemyNum );

	for(int i = 0; i < mEnemies.size(); i++) {
		mEnemies.at(i)->save(pDoc);
	}

	TiXmlElement * pickupNum = new TiXmlElement( "pickupNum" ); 
	pickupNum->LinkEndChild( new TiXmlText( Constants::convertInt(mPickups.size()).c_str() ));
	pDoc.RootElement()->LinkEndChild( pickupNum );

	for(int i = 0; i < mPickups.size(); i++) {
		mPickups.at(i)->save(pDoc);
	}

	TiXmlElement * crateNum = new TiXmlElement( "crateNum" ); 
	crateNum->LinkEndChild( new TiXmlText( Constants::convertInt(mCrates.size()).c_str() ));
	pDoc.RootElement()->LinkEndChild( crateNum );

	for(int i = 0; i < mCrates.size(); i++) {
		mCrates.at(i)->save(pDoc);
	}

	TiXmlElement * curLevel = new TiXmlElement( "thisLevel" ); 
	pDoc.RootElement()->LinkEndChild( curLevel );

	TiXmlElement* name = new TiXmlElement( "name" );  
	name->LinkEndChild( new TiXmlText( mCurLevel.c_str() ));
	curLevel->LinkEndChild( name );

	TiXmlElement * nextLevel = new TiXmlElement( "nextLevel" ); 
	pDoc.RootElement()->LinkEndChild( nextLevel );

	TiXmlElement* nextLevelName = new TiXmlElement( "name" );  
	nextLevelName->LinkEndChild( new TiXmlText( mNextLevel.c_str() ));
	curLevel->LinkEndChild( nextLevelName );
}

/**
* Destructor
*/
Level::~Level() {
	
	delete mWorld;		mWorld = NULL;
}