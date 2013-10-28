#include "game.h"

s3eInetAddress Game::mDest_addr;
s3eSocket* Game::mSock;
s3eInetAddress Game::mMy_addr;
bool Game::mHost;
char* Game::mRequestBuffer;
int Game::mPlayersInLobby;
bool Game::mPlayerFound;
CIwHTTP* Game::mHttpObject;
s3eFBSession* Game::g_CurrentSession = 0;
Constants::s3eFBStatus Game::g_SessionStatus = Constants::FB_UNINITIALISED;
Constants::GameStage Game::mStage;
CIwStringS Game::ServerUrl;
string Game::g_UID;
s3eFBRequest* Game::g_GetUID;
s3eFBRequest* Game::g_AddScore;
Level* Game::mLevel;
std::map<std::string, int> Game::g_Scores;
int Game::g_SelectedLvlScore;
std::string Game::g_Selectedlvl;
MenuSystem* Game::mMenuSystem;
int Game::g_Score;
s3eFBRequest* Game::PostScoreRequest;

float Game::mTimeTaken;
int Game::mPlayerDeaths;
std::string Game::g_Token;

bool Game::crateSpawned;
int Game::maxCrates;

/**
* Constructor
*/
Game::Game():

	physicsHz(60),
	timeStep(1.0f/physicsHz),
	velocityIterations(10),
	positionIterations(8),
	g_accumulator(0.0f),
	g_prevTime(s3eTimerGetMs()),
	loadCheckpoint(false), 
	mPaused(false),
	mMultiplayer(false),
	mBytesRecieved(-1),
	timeSinceLastSend(0),
	mRDXVel(0),
	mRecRDXVel(0),
	mDiff(1),
	mTimeSinceLastRequest(0),
	mSelectedLevel("1"),
	mPlayersFinished(0),
	FACEBOOK_APP_ID("160040267492274"),
	FACEBOOK_APP_SECRET("b7d69cf0ef9e50a344d892df3c8c7fe6") {

		mRequestBuffer = NULL;
		mHost = true;
		mSock = NULL;
		mPlayerFound = false;
		mPlayersInLobby = 0;
		mStage = Constants::MAINMENU;
		ServerUrl = "http://10.40.3.237:28003";
		g_GetUID = NULL;
		g_AddScore = NULL;
		mLevel = NULL;
		g_SelectedLvlScore = 0;
		g_Selectedlvl = "tutorial";
		PostScoreRequest = NULL;
		mTimeTaken = 0.0f;
		mPlayerDeaths = 0;
		g_CurrentSession = NULL;

		crateSpawned = true;
		maxCrates = 1;

		moveLeftBtn = Iw2DCreateImage("Images/ArrowLeft.png");
		moveRightBtn = Iw2DCreateImage("Images/ArrowRight.png");
		jumpBtn = Iw2DCreateImage("Images/ArrowJump.png");
		kickBtn = Iw2DCreateImage("Images/kick.png");
		punchBtn = Iw2DCreateImage("Images/punch.png");

		mMenuSystem = new MenuSystem();
		mMenuSystem->init(this);

		mCollListener = new MyContactListener();
		mCollListener->setGame(this);

		// Prepare the iwgxfont resource for rendering using Iw2D
		mFont = Iw2DCreateFontResource("font_large");

		if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3)) {
			s3eAudioPlay("Audio/MainMenu.mp3", 0);
		}

		mMusic.push_back("Audio/gameMusic1.mp3");
		mMusic.push_back("Audio/gameMusic2.mp3");

		srand((unsigned)time(0));
}

/**
* Game update function
* @param pStream the packet stream to write state data to (if playing multiplayer)
*/
void Game::update(PacketStream* pStream) {

	//cout << "update" << endl;

	// timer
	g_timeNow = s3eTimerGetMs();
	g_deltaTime = float( (g_timeNow - g_prevTime) * 0.001 );
	// Time since last packet was sent
	timeSinceLastSend += g_timeNow - g_prevTime;
	g_prevTime = g_timeNow;

	if(mStage == Constants::INGAME) {

		for(int i = 0; i < bufferSize; i++) {
	
			mMessageReceived[i] = 0;
			mMessageSent[i] = 0;
		}

		if(mPlayer->getHealth() <= 0) {
			mPlayer->loadCheckpoint = true;
			mPlayer->mPos = CIwSVec2(mCheckpoint.x, mCheckpoint.y);

			// Create respawn effect
			CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);
			CIwSVec2 pos = screenCentre + (CIwSVec2(int16(mCheckpoint.x*8), -int16(mCheckpoint.y*8)));

			createEffect(CIwSVec2(pos.x, pos.y), 10, "Images/respawn.png", 1.0f);
		}

		if(mSpawnPickup == true) {
		
			mLevel->createPickup(mPickupSpawnType, mPickupSpawnPoint.x, mPickupSpawnPoint.y);
			mSpawnPickup = false;
		}

		if(!mPaused) {
			mTimeTaken += g_deltaTime;
		}

		// Sync clients if multiplayer
		if(mMultiplayer) {

			// Packet rate modification
			if(timeSinceLastSend >= 1000/30) {

				pStream->writeInt(Constants::UPDATE);

				// Dead reckoning
				if( abs(mDRPlayer->getBody()->GetPosition().Length() - mPlayer->getBody()->GetPosition().Length()) >= 1 ) {

					pStream->writeInt(Constants::DRUPDATE);
					mPlayer->serialize(pStream);
					mRDXVel = mPlayer->getBody()->GetLinearVelocity().x;

					mDRPlayer->getBody()->SetTransform(mPlayer->getBody()->GetPosition(), 0.0f);
					mDRPlayer->getBody()->SetLinearVelocity(b2Vec2(mRDXVel, mPlayer->getBody()->GetLinearVelocity().y));
				}
				else {
					pStream->writeInt(Constants::UPDATE);
				}

				if(mHost) {
					// Serialize level
					mLevel->serialize(pStream);
				}

				pStream->writeChar('\0');
				pStream->toCharArray(mMessageSent);
				mMessageSent[strlen(mMessageSent)-1] = '\0';

				//cout << "SENDING: " << mMessageSent << endl;
				// Send data
				s3eSocketSendTo(mSock, mMessageSent, strlen(mMessageSent), 0, &mDest_addr);

				timeSinceLastSend = 0;
			}

			// Recieve data
			mBytesRecieved = s3eSocketRecvFrom(mSock, mMessageReceived, sizeof(mMessageReceived), 0, &mDest_addr);
		

			if(mBytesRecieved != -1) {
				cout << "RECIEVING: " << mMessageReceived << endl;
				pStream->fromCharArray(mMessageReceived);

				int type;
				pStream->readInt(type);

				while (type) {
					cout << "checking message types" << endl;
					if(type == Constants::UPDATE) {

						int updateType;
						pStream->readInt(updateType);

						if(updateType == Constants::DRUPDATE) {

							mOtherPlayer->deserialize(pStream);
							mRecRDXVel = mOtherPlayer->getBody()->GetLinearVelocity().x;
						}
						else {
							if(!mHost) {
								// Deserialize the level
								mLevel->deserialize(pStream);
							}
						}
					}
					else if(type == Constants::DESTROY) {
					
						int id;
						pStream->readInt(id);

						mLevel->killEnemy(id);
					}
					else if(type == Constants::DESTROYPICKUP) {
					
						int id;
						pStream->readInt(id);

						mLevel->destroyPickup(id);
					}
					else if(type == Constants::CREATEPICKUP) {
					
						int type, x, y, id;
						pStream->readInt(type);
						pStream->readInt(id); // Clients sends -1
						pStream->readInt(x);
						pStream->readInt(y);

						mLevel->createPickup(type, x, y);

						if(mHost) {
							// Send create to peer
							// Send Id of created pickup
							PacketStream s;
							s.writeInt(Constants::CREATEPICKUP);
							s.writeInt(mLevel->getPickups().back()->getID());
							s.writeInt(type);
							s.writeFloat(x);
							s.writeFloat(y);
							s.writeChar('\0');

							char message[10];
							s.toCharArray(message);
							message[strlen(message)-1] = '\0';

							s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
						}
						else {
							// Set id of pickup if client if client
							mLevel->getPickups().back()->setID(id);
						}
					}
					else if(type == Constants::CREATECRATE) {
				
						int type, id;
						float x, y;
						//pStream->readInt(id);
						pStream->readInt(type);
						pStream->readFloat(x);
						pStream->readFloat(y);

						if(mHost) {
						
							mLevel->createCrate(type, x, y);
							crateSpawned = true;

							PacketStream s;
							s.writeInt(Constants::CREATECRATE);
							//s.writeInt(mLevel->getCrates().back()->getID());
							s.writeInt(type);
							s.writeFloat(x);
							s.writeFloat(y);
							s.writeChar('\0');

							char message[10];
							s.toCharArray(message);
							message[strlen(message)-1] = '\0';

							//s3eSocketSendTo(mSock, message, strlen(message), 0, mDest_addr);
						}
						else {
							mLevel->createCrate(type, x, y);
							//mLevel->getCrates().back()->setID(id);
							crateSpawned = true;
						}
					}
					else if(type == Constants::TOGGLEPLATFORM) {
						int id;

						pStream->readInt(id);
						mLevel->getActivatablePlatforms().at(id)->toggle();
					}
					else if(type == Constants::ACTION) {

						int action;
						pStream->readInt(action);

						//Punch
						if(action == 0) {
						
							// Play punch animation
							mOtherPlayer->punch();

							vector<Enemy*> enemies = mLevel->getEnemies();
							b2Body* player = mOtherPlayer->getBody();

							for(int i = 0; i < enemies.size(); i++) {				
								if( abs((enemies.at(i)->getBody()->GetPosition() - player->GetPosition()).Length()) <= 10) {
									short direction = 1;

									if(enemies.at(i)->getBody()->GetPosition().x < player->GetPosition().x) {
										direction = -1;
									}

									b2Vec2 force = b2Vec2(50000*direction, 0);
									enemies.at(i)->getBody()->ApplyForce(force, enemies.at(i)->getBody()->GetWorldCenter());
									enemies.at(i)->ApplyDamage(mOtherPlayer->getMeleeStr());

									CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

									b2Transform t = enemies.at(i)->getBody()->GetTransform();
									CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

									createEffect(pos, 8 , "Images/blood.png", 1.0f);
								}
							}
						}
						//Kick
						else if(action == 1) {
						
							// Play kick animation
							mOtherPlayer->kick();

							vector<Enemy*> enemies = mLevel->getEnemies();
							b2Body* player = mOtherPlayer->getBody();

							for(int i = 0; i < enemies.size(); i++) {				
								if( abs((enemies.at(i)->getBody()->GetPosition() - player->GetPosition()).Length()) <= 10) {
									short direction = 1;

									if(enemies.at(i)->getBody()->GetPosition().x < player->GetPosition().x) {
										direction = -1;
									}

									b2Vec2 force = b2Vec2(500000*direction, 500000);
									enemies.at(i)->getBody()->ApplyForce(force, enemies.at(i)->getBody()->GetWorldCenter());
								}
							}
						}
					}
					else if(type == Constants::SWITCH) {
					
						int id;
						mLevel->getSwitches().at(id)->toggle();
					}

					// Get next message type
					pStream->readInt(type);
					if(type > Constants::SWITCH || type < Constants::UPDATE) {

						type = 0;
					}
				}
			}
		}
		
		if(!mPaused) {

			// physics loop (fixed timing at 60Hz)
			g_accumulator += g_deltaTime;

			while(g_accumulator > 0.0f)
			{
				//cout << "world step" << endl;
				mLevel->getWorld()->Step(timeStep, velocityIterations, positionIterations);
				g_accumulator -= timeStep;
			}
			mLevel->getWorld()->DrawDebugData();
		}
		else {
			if(mMultiplayer) {
		
				// physics loop (fixed timing at 60Hz)
				g_accumulator += g_deltaTime;

				while(g_accumulator > 0.0f)
				{
					//cout << "world step" << endl;
					mLevel->getWorld()->Step(timeStep, velocityIterations, positionIterations);
					g_accumulator -= timeStep;
				}
				mLevel->getWorld()->DrawDebugData();
			}
		}

		CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

		b2Transform t = mPlayer->getBody()->GetTransform();
		CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x), -int16(t.p.y)));

		mLevel->drawBackground(CIwSVec2((pos.x*8)-(1024/2), (pos.y*8)-(768/2)), CIwSVec2(pos.x*4, pos.y*8));

		float angle = -t.q.GetAngle() * (180.0f/3.14159f);	// reverse angle as our screen-Y coord is reversed

		CIwMat2D rot;
		rot.SetRot(iwangle(angle * 1024 / 90), CIwVec2(pos) << 3);
		Iw2DSetTransformMatrixSubPixel(rot);

		iwangle angle1 = 0;
		// Set the rotation transform
		CIwMat2D m;
		m.SetRot(angle1);
		// Scale the transform
		m.ScaleRot(IW_GEOM_ONE);
		// Translate the transform
		m.SetTrans(CIwSVec2(-t.p.x*8, t.p.y*8));
		// Set this transform as the active transform for Iw2D
		Iw2DSetTransformMatrix(m);

		mLevel->update(CIwSVec2( mPlayer->getBody()->GetPosition().x, mPlayer->getBody()->GetPosition().y), pStream, mHost);

		if(mPaused == false) {

			mPlayer->update(g_deltaTime);
		
			if(mMultiplayer) {

				//mDRPlayer->getBody()->SetLinearVelocity(b2Vec2(mRDXVel, mPlayer->getBody()->GetLinearVelocity().y));
				//mDRPlayer->update();

				//mOtherPlayer->getBody()->SetLinearVelocity(b2Vec2(mRecRDXVel, mOtherPlayer->getBody()->GetLinearVelocity().y));
				mOtherPlayer->update(g_deltaTime);
			}
		}
		else {
			mPlayer->draw();
		}

		// Update effects
		for(int i = 0; i < mEffects.size(); i++) {

			mEffects.at(i)->update();

			// Remove finished effects
			if(mEffects.at(i)->isFinished()) {	

				delete mEffects.at(i);
				mEffects.erase(mEffects.begin() + i);
			}
		}

		Iw2DSetTransformMatrix(CIwMat2D::g_Identity);
		
		if(!mPaused && mPlayer->isFinished() == false) {

			CIwSVec2 btnSize(100, 100);
			// Draw ui buttons
			Iw2DDrawImage(moveLeftBtn, CIwSVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-220), btnSize);
			Iw2DDrawImage(moveRightBtn, CIwSVec2(120, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-110), btnSize);
			Iw2DDrawImage(jumpBtn, CIwSVec2(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-110, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-220), btnSize);
			Iw2DDrawImage(kickBtn, CIwSVec2(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-220, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-110), btnSize);
			Iw2DDrawImage(punchBtn, CIwSVec2(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-220, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-220), btnSize);
		}

		//b2Debug();

		/*CIwStringS time;

		time = "TIME TAKEN: " + CIwStringS(Constants::convertInt(mTimeTaken).c_str()) + " SECONDS";
		Iw2DSetFont(mFont);
		// Draw in black
		CIwColour col;
		col.Set(255, 255, 255);
		Iw2DSetColour(col);

		//Draw text
		// Render the text into a 100x100 region
		CIwSVec2 region(500, 50);

		// Centred on the centre of the surface
		CIwSVec2 topLeft((int16)(Iw2DGetSurfaceWidth()/2 - region.x/2),
		(int16)(Iw2DGetSurfaceHeight()/2 - region.y/2));
		// Draw the string into the region
		Iw2DDrawString(time.c_str(), topLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

		col.Set(255, 255, 255);
		Iw2DSetColour(col);*/
	}
	else if(mStage == Constants::MAINMENU) {
	
		if(g_SessionStatus == Constants::FB_LOGGED_IN) {
			//cout << "update" << endl;
		}
	}
	else if(mStage == Constants::LEVELSELECT) {
	
		// Set the current font
		Iw2DSetFont(mFont);
		// Draw in black
		CIwColour col;
		col.Set(0, 0, 0);
		Iw2DSetColour(col);

		//Draw text
		CIwSVec2 region(500, 50);

		// Centred on the centre of the surface
		CIwSVec2 levelTopLeft((Iw2DGetSurfaceWidth()/2)-250, (Iw2DGetSurfaceHeight()-200));
		CIwSVec2 scoreTopLeft((Iw2DGetSurfaceWidth()/2)-250, (Iw2DGetSurfaceHeight()-150));

		CIwStringS levelName = CIwStringS("Selected level: ") + CIwStringS(g_Selectedlvl.c_str());
		Iw2DDrawString(levelName.c_str(), levelTopLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

		if(g_SessionStatus == Constants::FB_LOGGED_IN) {

			int number = g_SelectedLvlScore;
			stringstream strs;
			strs << number;
			string temp_str = strs.str();

			CIwStringS score = CIwStringS("Score: ") + CIwStringS(temp_str.c_str());

			// Draw the string into the region
			
			Iw2DDrawString(score.c_str(), scoreTopLeft + CIwSVec2(0, region.y), region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
		}
		else {

			// Draw the string into the region
			Iw2DDrawString("Login required to retrieve score data", scoreTopLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
		}

		col.Set(255, 255, 255);
		Iw2DSetColour(col);
	}
	else if(mStage == Constants::ENDLEVEL) {
	
		int Difficulty = 1;

		// Set the current font
		Iw2DSetFont(mFont);
		CIwColour col;
		col.Set(255, 255, 255);
		Iw2DSetColour(col);

		//Draw text
		// Render the text into a 100x100 region
		CIwSVec2 region(500, 50);

		// Centred on the centre of the surface
		CIwSVec2 topLeft((int16)(Iw2DGetSurfaceWidth()/2 - region.x/2),
		(int16)(Iw2DGetSurfaceHeight()/2 - region.y/2));

		CIwStringS time, deaths, difficulty;

		time = "TIME TAKEN: " + CIwStringS(Constants::convertInt(mTimeTaken).c_str()) + " SECONDS";
		deaths = "DEATHS: " + CIwStringS(Constants::convertInt(mPlayerDeaths).c_str());

		if(Difficulty == 0.5) {
			difficulty = "DIFFICULTY: Easy";
		}
		else if(Difficulty == 1) {
			difficulty = "DIFFICULTY: Normal";
		}
		else if(Difficulty == 5) {
			difficulty = "DIFFICULTY: Hard";
		}

		// Draw the string into the region
		Iw2DDrawString(time.c_str(), topLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
		Iw2DDrawString(deaths.c_str(), topLeft + CIwSVec2(0, region.y), region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
		Iw2DDrawString(difficulty.c_str(), topLeft + CIwSVec2(0, region.y*2), region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);

		col.Set(255, 255, 255);
		Iw2DSetColour(col);
	}
	else if(mStage == Constants::LOBBY) {

		lobbyUpdate(pStream, g_deltaTime);
	}
}

/**
* Update function called while in the multiplayer lobby
* @param pStream the packet stream to write updates to
* @param pdeltaTime the time since the last frame
*/
void Game::lobbyUpdate(PacketStream* pStream, float pDeltaTime) {

	// In multiplayer lobby
	CIwStringS players, level;
	players = "Players Joined: " + CIwStringS(Constants::convertInt(mPlayersInLobby).c_str());
	level = CIwStringS("Selected Level: ") + CIwStringS(g_Selectedlvl.c_str());

	// Set the current font
	Iw2DSetFont(mFont);
	// Draw in black
	CIwColour col;
	col.Set(0, 0, 0);
	Iw2DSetColour(col);

	//Draw text
	CIwSVec2 region(500, 50);

	// Centred on the centre of the surface
	CIwSVec2 playerTopLeft((Iw2DGetSurfaceWidth()/2)-250, (Iw2DGetSurfaceHeight()-200));
	CIwSVec2 levelTopLeft((Iw2DGetSurfaceWidth()/2)-250, (Iw2DGetSurfaceHeight()-150));

	Iw2DDrawString(players.c_str(), playerTopLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	Iw2DDrawString(level.c_str(), levelTopLeft, region, IW_2D_FONT_ALIGN_CENTRE, IW_2D_FONT_ALIGN_CENTRE);
	col.Set(255, 255, 255);
	Iw2DSetColour(col);

	char messageRec[50];
	int bytesRec = -1;
	bytesRec = s3eSocketRecvFrom(mSock, messageRec, sizeof(messageRec), 0, &mDest_addr);

	if(mHost) {
		mTimeSinceLastRequest += pDeltaTime;

		if(mTimeSinceLastRequest > 100/30 && mPlayerFound == false) {
			
			mHttpObject->Get(CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/getSession")).c_str(), GotGetData, NULL);
			mTimeSinceLastRequest = 0;
		}

		if(mPlayerFound == true) {
				
			pStream->writeInt(Constants::SELECTLEVEL);
			pStream->writeChar('1');
		}

		if(bytesRec != -1) {
			pStream->fromCharArray(messageRec);
			int type;
			pStream->readInt(type);

			while (type) {

				if(type == Constants::READY) {
					// client is ready show start button
				}
				/*else if(type == Constants::STARTGAME) {
					// Start game
					loadMultLevel(selectedLevel);
					hostOptions->SetVisible(false);
				}*/

				pStream->readInt(type);
				if(type > Constants::SWITCH || type < Constants::UPDATE) {

					type = 0;
				}
			}
		}
	}
			
	// host
	// Send start game message

	if(!mHost) {
			
		if(bytesRec != -1) {
			// Start game
			// loadLevel(selectedLevel);
			//mStage = Constants::INGAME;
			//cout << "start Message" << endl;
			mMenuSystem->clearMenus();
			loadMultLevel(g_Selectedlvl.c_str());					
			//hostOptions->SetVisible(true);

			// WHAT USED TO BE HERE

			/*pStream->fromCharArray(messageRec);
			int type;
			pStream->readInt(type);

			while (type) {

				cout << "lobby message types" << endl;

				if(type == Constants::SELECTLEVEL) {
					// Set selected level
					char levelNum;
					pStream->readChar(&levelNum);
					mSelectedLevel = (char*)levelNum;
				}
				else if(type == Constants::STARTGAME) {
					// Start game
					// loadLevel(selectedLevel);
					//mStage = Constants::INGAME;
					cout << "start Message" << endl;
					mMenuSystem->clearMenus();
					loadMultLevel(g_Selectedlvl.c_str());					
					//hostOptions->SetVisible(true);
				}

				pStream->readInt(type);
				if(type > Constants::SWITCH || type < Constants::UPDATE) {

					type = 0;
				}
			}*/

		}
	}
}

/**
* Sets the current stage of the game (Ingame, in menu, etc.)
* @param the stage to set as
*/
void Game::setStage(Constants::GameStage pStage) {
	mStage = pStage;
}

/**
* Creates game objects at runtime and sends a create message to other players in multiplayer
* @param pMsgType the type of message to send to other players (eg. CREATECRATE, CREATEPICKUP)
* @param pObjID the id number to give the created object
* @param xPos the x position at which too create the object
* @param yPos the y position at which too create the object
*/
void Game::create(int pMsgType, int pObjID, float xPos, float yPos) {

	if(pMsgType == Constants::CREATECRATE) {
		
		if(crateSpawned == false) {

			mLevel->createCrate(pObjID, xPos, yPos);
			crateSpawned = true;

			PacketStream s;
			s.writeInt(Constants::CREATECRATE);
			//s.writeInt(mGame->getLevel()->getCrates().back()->getID());
			s.writeInt(pObjID);
			s.writeFloat(xPos);
			s.writeFloat(yPos);
			s.writeChar('\0');

			char message[10];
			s.toCharArray(message);
			message[strlen(message)-1] = '\0';

			s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
		}
	}
	else if(pMsgType == Constants::CREATEPICKUP) {
	
		mPickupSpawnType = pObjID;
		mPickupSpawnPoint = b2Vec2(xPos, yPos);
		mSpawnPickup = true;
	}
	else if(pMsgType == Constants::TOGGLEPLATFORM) { 

		PacketStream s;
		s.writeInt(Constants::TOGGLEPLATFORM);
		s.writeInt(pObjID);
		s.writeChar('\0');

		char message[10];
		s.toCharArray(message);
		message[strlen(message)-1] = '\0';

		s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
	}
	else if(pMsgType == Constants::ACTION) { 
		
		PacketStream s;
		s.writeInt(Constants::ACTION);
		s.writeInt(pObjID);
		s.writeChar('\0');

		char message[10];
		s.toCharArray(message);
		message[strlen(message)-1] = '\0';

		s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
	}
}

/**
* Gets the current facebook status ( logged in, logged out )
* @return the current facebook status
*/
Constants::s3eFBStatus Game::getFBStatus() {
	return g_SessionStatus;
}

/**
* Checks if this game is the host in multiplayer
* @return true if this game is host, else false
*/
bool Game::isHost() {
	return mHost;
}

/**
* sends a toggle switdch message to other playres in multiplayer
* @param pID the id of the switch to toggle 
*/
void Game::toggleSwitch(int pID) {

	PacketStream s;
	s.writeInt(Constants::SWITCH);
	s.writeInt(pID);
	s.writeChar('\0');

	char message[10];
	s.toCharArray(message);
	message[strlen(message)-1] = '\0';

	s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
}

/**
* Gets the scores for each level if player is logged into facebook
*/
void Game::getScores() {

	if(g_SessionStatus == Constants::FB_LOGGED_IN) {

		if(mHttpObject == NULL) {
			mHttpObject = new CIwHTTP;
		}

		g_Scores.clear();

		CIwStringL url;
		url = CIwStringL(ServerUrl.c_str()) + CIwStringL("/getScores/") + CIwStringL(g_UID.c_str());

		mHttpObject->Get(url.c_str(), GotScoreData, NULL);
	}
}

/**
* callback, called after sending a http post request to the game server
*/
int32 Game::GotPostData(void*, void*)
{
	char **resultBuffer = &mRequestBuffer;

    *resultBuffer = (char*)s3eMalloc(200 + 1);
    (*resultBuffer)[200] = 0;
    mHttpObject->ReadData(*resultBuffer, 200);

	if(mRequestBuffer != NULL) {

		TiXmlDocument * doc;
		doc = new TiXmlDocument();
		doc->Parse((const char*)mRequestBuffer, 0, TIXML_ENCODING_UTF8);
		TiXmlElement * root = doc->RootElement();
		TiXmlElement * playerRoot = root->FirstChildElement("player");
		TiXmlElement * info;
		playerRoot = playerRoot->NextSiblingElement();

		mSock = s3eSocketCreate(S3E_SOCKET_UDP, 0);

		CIwStringS myIP, theirIP;
		int myPort, theirPort;

		if(playerRoot == NULL) {
			// Only one player, 
			// you have created a game,
			// you are host
			mHost = true;
			playerRoot = root->FirstChildElement("player");
			info = playerRoot->FirstChildElement("ip");
			myIP = info->FirstChild()->Value();
			info = playerRoot->FirstChildElement("port");
			myPort = atof(info->FirstChild()->Value());		

			memset(&mMy_addr, 0, sizeof(mMy_addr));
			mMy_addr.m_Port = s3eInetHtons(myPort);
			mMy_addr.m_Abstract=0x0;
			mMy_addr.m_Local=0x0;
			mMy_addr.m_IPAddress = 0;
			s3eInetAton(&mMy_addr.m_IPAddress, myIP.c_str());

			bool result = s3eSocketBind(mSock, &mMy_addr, S3E_TRUE);

			mPlayersInLobby = 1;
			mMenuSystem->showLobbyOptions(true);
		}
		else {
			// Two players
			// Joined game
			mHost = false;
			playerRoot = root->FirstChildElement("player");
			info = playerRoot->FirstChildElement("ip");
			theirIP = info->FirstChild()->Value();
			info = playerRoot->FirstChildElement("port");
			theirPort = atof(info->FirstChild()->Value());
			playerRoot = playerRoot->NextSiblingElement();
			info = playerRoot->FirstChildElement("ip");
			myIP = info->FirstChild()->Value();
			info = playerRoot->FirstChildElement("port");
			myPort = atof(info->FirstChild()->Value());

			memset(&mMy_addr, 0, sizeof(mMy_addr));
			mMy_addr.m_Port = s3eInetHtons(myPort);
			mMy_addr.m_Abstract=0x0;
			mMy_addr.m_Local=0x0;
			mMy_addr.m_IPAddress = NULL;
			s3eInetAton(&mMy_addr.m_IPAddress, myIP.c_str());

			bool result = s3eSocketBind(mSock, &mMy_addr, S3E_TRUE);
			s3eSocketGetLocalName(mSock, &mMy_addr);

			memset(&mDest_addr, 0, sizeof(mDest_addr));
			mDest_addr.m_Port = s3eInetHtons(theirPort);
			mDest_addr.m_IPAddress = 0;
			s3eInetAton(&mDest_addr.m_IPAddress, theirIP.c_str());

			mPlayersInLobby = 2;
			mMenuSystem->showLobbyOptions(false);
		}

		delete mRequestBuffer;
		mRequestBuffer = NULL;
	}

    return 0;
}

/**
* callback, called after sending a http get request to the game server
*/
int32 Game::GotGetData(void*, void*)
{
	char **resultBuffer = &mRequestBuffer;

    *resultBuffer = (char*)s3eMalloc(200 + 1);
    (*resultBuffer)[200] = 0;
    mHttpObject->ReadData(*resultBuffer, 200);

	if(mRequestBuffer != NULL) {

		CIwStringS myIP, theirIP;
		int myPort, theirPort;

		TiXmlDocument * doc;
		doc = new TiXmlDocument();
		doc->Parse((const char*)mRequestBuffer, 0, TIXML_ENCODING_UTF8);
		TiXmlElement * root = doc->RootElement();
		TiXmlElement * playerRoot = root->FirstChildElement("player");
		TiXmlElement * info;
		playerRoot = playerRoot->NextSiblingElement();

		if(playerRoot == NULL) {
			// Only one player, 
		}
		else {
			// Two players
			info = playerRoot->FirstChildElement("ip");
			theirIP = info->FirstChild()->Value();
			info = playerRoot->FirstChildElement("port");
			theirPort = atof(info->FirstChild()->Value());

			memset(&mDest_addr, 0, sizeof(mDest_addr));
			mDest_addr.m_Port = s3eInetHtons(theirPort);
			mDest_addr.m_IPAddress = 0;
			s3eInetAton(&mDest_addr.m_IPAddress, theirIP.c_str());

			mPlayerFound = true;
			mPlayersInLobby = 2;

			/*char message[10];
			PacketStream s;
			s.writeInt(Constants::SELECTLEVEL);
			s.writeChar('1');
			s.writeChar('\0');
			s.toCharArray(message);
			message[strlen(message)-1] = '\0';
			// Send data
			s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);*/
		}

		delete mRequestBuffer;
		mRequestBuffer = NULL;
	}

    return 0;
}

/**
* callback, called after sending a http get request to the game server asking for score data
*/
int32 Game::GotScoreData(void*, void*)
{
	char **resultBuffer = &mRequestBuffer;

    *resultBuffer = (char*)s3eMalloc(300 + 1);
    (*resultBuffer)[300] = 0;
    mHttpObject->ReadData(*resultBuffer, 300);

	if(mRequestBuffer != NULL) {

		std::string levelName;
		int score;

		TiXmlDocument * doc;
		doc = new TiXmlDocument();
		doc->Parse((const char*)mRequestBuffer, 0, TIXML_ENCODING_UTF8);
		TiXmlElement * root = doc->RootElement();
		TiXmlElement * scoreRoot = root->FirstChildElement("scoreInfo");
		TiXmlElement * info;

		while( scoreRoot != NULL) {
			
			info = scoreRoot->FirstChildElement("levelName");
			levelName = (char*)info->FirstChild()->Value();
			info = scoreRoot->FirstChildElement("score");
			score = atoi(info->FirstChild()->Value());

			g_Scores.insert(pair<std::string, int>(levelName, score));
			scoreRoot = scoreRoot->NextSiblingElement();
		}

		g_SelectedLvlScore = g_Scores.find(g_Selectedlvl)->second;
	}

    return 0;
}

/**
* Loads a multiplayer level
* @param pLvl the name of the level to load
*/
void Game::loadMultLevel(CIwStringS pLvl) {

	CIwStringS levelName = pLvl;

	mPaused = false;
	mMultiplayer = true;

	loadLevel(levelName);
	//mCheckpoint = mPlayer->getBody()->GetPosition();
	
	//cout << "menus cleared" << endl;
	/*if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3)) {
		s3eAudioPlay("Audio/gameMusic.mp3", 0);
	}*/
}

/**
* Gets the most recent checkpoint
* @param the position of the most recent checkpoint
*/
b2Vec2 Game::getCheckPoint() {
	return mCheckpoint;
}

/**
* Sets the current checkpoint position
* @param the position to set as the checkpoint
*/
void Game::setCheckPoint(b2Vec2 pCheckpoint) {
	mCheckpoint = pCheckpoint;
}

/**
* Checks if the current game is a multiplayer game
* @return true if game is multiplayer, else false
*/
bool Game::isMultiplayer() {
	return mMultiplayer;
}

/**
* Sets the position at which to spawn a pickup
* @param pPos the poaition at which to spawn the pickup
*/
void Game::setPickupSpawnPoint(b2Vec2 pPos) {
	mPickupSpawnPoint = pPos;
}

/**
* Sets a flag for whether or not the game should spawn a pickup next update
* @param pVal the value to set the flag
*/
void Game::spawnPickup(bool pVal) {
	mSpawnPickup = pVal;
}

/**
* loads a level
* @param pLevel the name of the level to load
*/
void Game::loadLevel(CIwStringS pLevel) {

	mMenuSystem->showPauseBtn();
	//cout << "load start" << endl;
	
	b2Vec2					gravity(0.0f, -15.0f);
	bool					doSleep = true;

	mLevel = new Level(gravity, doSleep, pLevel, false, mDiff);
	//Set the contact Listener
	mLevel->getWorld()->SetContactListener(mCollListener);

	float xPos, yPos, damage;
	CIwStringS dataFile = "Levels/" + pLevel + "Data.xml";
	TiXmlDocument * doc;
	doc = new TiXmlDocument();
	doc->LoadFile(dataFile.c_str());

	TiXmlElement * root = doc->RootElement();
	TiXmlElement * playerRoot;
	TiXmlElement * otherPlayerRoot;

	if(mHost) {
		playerRoot = root->FirstChildElement("player");
		otherPlayerRoot = root->FirstChildElement("player2");
	}
	else {
		playerRoot = root->FirstChildElement("player2");
		otherPlayerRoot = root->FirstChildElement("player");
	}

	TiXmlElement * info;
	info = playerRoot->FirstChildElement("xPos");
	xPos = atof(info->FirstChild()->Value());
	info = playerRoot->FirstChildElement("yPos");
	yPos = atof(info->FirstChild()->Value());
	info = playerRoot->FirstChildElement("damage");
	damage = atof(info->FirstChild()->Value());

	std::string myTex;// = "Images/multPlayer1.png";
	std::string theirTex;

	if(mMultiplayer) {

		if(mHost) {
			myTex = "Images/multPlayer1.png";
			theirTex = "Images/multPlayer2.png";
		}
		else {
			myTex = "Images/multPlayer2.png";
			theirTex = "Images/multPlayer1.png";
		}
	}
	else {
		
		myTex = "Images/player.png";
		theirTex = "Images/multPlayer2.png";
	}

	mPlayer = new Player(myTex.c_str(), b2Vec2(xPos, yPos), mLevel->getWorld(), false);
	mPlayer->applyDamage(damage);

	if(mMultiplayer) {

		info = otherPlayerRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = otherPlayerRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());
		info = playerRoot->FirstChildElement("damage");
		damage = atof(info->FirstChild()->Value());
		
		mDRPlayer = new Player("Images/playerDR.png", b2Vec2(xPos, yPos), mLevel->getWorld(), false);
		mOtherPlayer = new Player(theirTex.c_str(), b2Vec2(xPos, yPos), mLevel->getWorld(), false);
		mOtherPlayer->applyDamage(damage);
	}

	mCheckpoint = b2Vec2(xPos, yPos);
	mPlayer->mPos = CIwSVec2(mCheckpoint.x, mCheckpoint.y);
	delete doc;

	vector<Enemy*> enemies = mLevel->getEnemies();

	for(int i = 0; i < enemies.size(); i++) {
	
		enemies.at(i)->setPlayer(mPlayer);
	}

	mStage = Constants::INGAME;
	mTimeTaken = 0;
	mPaused = false;
	//cout << "load end" << endl;

	int random_integer = (rand()%2); 

	if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3)) {
		s3eAudioPlay(mMusic.at(random_integer).c_str(), 0);
	}

	mMenuSystem->clearMenus();
}

/**
* Loads the most recent saved game
*/
void Game::loadSave() {
	
	b2Vec2					gravity(0.0f, -15.0f);
	bool					doSleep = true;

	TiXmlDocument * doc;
	doc = new TiXmlDocument();
	doc->LoadFile("save.xml");

	TiXmlElement * root = doc->RootElement();

	if(root != NULL) {

		TiXmlElement * levelRoot = root->FirstChildElement("thisLevel");
		TiXmlElement * info;
		info = levelRoot->FirstChildElement("name");

		CIwStringS lvl;
		lvl = info->FirstChild()->Value();

		mLevel = new Level(gravity, doSleep, lvl, true, mDiff);
		mLevel->getWorld()->SetContactListener(mCollListener);

		float xPos, yPos;
		levelRoot = root->FirstChildElement("player");
		info = levelRoot->FirstChildElement("xPos");
		xPos = atof(info->FirstChild()->Value());
		info = levelRoot->FirstChildElement("yPos");
		yPos = atof(info->FirstChild()->Value());

		mPlayer = new Player("Images/player.png", b2Vec2(xPos, yPos), mLevel->getWorld(), true);

		mCheckpoint = b2Vec2(xPos, yPos);
		mPlayer->mPos = CIwSVec2(mCheckpoint.x, mCheckpoint.y);
		delete doc;

		vector<Enemy*> enemies = mLevel->getEnemies();

		for(int i = 0; i < enemies.size(); i++) {
	
			enemies.at(i)->setPlayer(mPlayer);
		}

		mStage = Constants::INGAME;
		mTimeTaken = 0;
		mPaused = false;
		mMultiplayer = false;

		int random_integer = (rand()%2); 

		if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3)) {
			s3eAudioPlay(mMusic.at(random_integer).c_str(), 0);
		}

		mMenuSystem->clearMenus();
		mMenuSystem->showPauseBtn();
	}
	else {
		loadLevel("tutorial");
	}
}

/**
* Sends a request to the game server asking for multiplayer agme sessions
*/
void Game::findMultiplayerSession() {

	CIwStringL body;

	body = CIwStringL(Constants::convertInt(28001).c_str());
	mHttpObject = new CIwHTTP();
	mHttpObject->Post( CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/searchForSession")).c_str(), body.c_str(), body.length(), GotPostData, NULL);

	mStage = Constants::LOBBY;
}

/**
* Ends the current level and displays the score screen
*/
void Game::endLevel() {
	
	mMenuSystem->ShowEndLevelScreen();
	mPaused = true;
	mStage = Constants::ENDLEVEL;

	if(mHttpObject == NULL) {
		mHttpObject = new CIwHTTP();
	}

	if(Game::g_SessionStatus == Constants::FB_LOGGED_IN) {
		
		// Send score
		g_AddScore = s3eFBRequest_WithGraphPath(g_CurrentSession, "me");
		s3eResult result = s3eFBRequest_Send(g_AddScore, sendScoreCallback);
	}
}

/**
* destroys the current level
*/
void Game::destroyLevel() {

	if(mLevel != NULL) {

		delete mLevel;
		mLevel = NULL;
	}
	else {
		
		if(mHttpObject == NULL) {
			mHttpObject = new CIwHTTP();
		}

		mHttpObject->Post( CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/destroySession")).c_str(), g_UID.c_str(), g_UID.length(), NULL, NULL);
	}
	mPaused = false;
	mPlayerFound = false;
	mPlayersInLobby = 0;
	mMultiplayer = false;
	mStage = Constants::MAINMENU;

	if (s3eAudioIsCodecSupported(S3E_AUDIO_CODEC_MP3)) {
		s3eAudioPlay("Audio/MainMenu.mp3", 0);
	}
}

/**
* shows tutorial message. Called in the collision manager when the player collides with a message sensor.
* @param pMsgID the message to show
*/
void Game::showMessage(const char* pMsgID) {

	mMenuSystem->showMessage(pMsgID);
}

/**
* Send a start game message to other players in a mutliplayer game lobby
*/
void Game::sendStartMsg() {

	if(mHttpObject == NULL) {
		mHttpObject = new CIwHTTP;
	}

	mHttpObject->Get(CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/destroySession")).c_str(), NULL, NULL);

	//mStage = Constants::INGAME;
	PacketStream s;
	s.writeInt(Constants::STARTGAME);
	s.writeChar('\0');

	char message[10];
	s.toCharArray(message);
	message[strlen(message)-1] = '\0';

	s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
}

/**
* Callback, called when the player logs into facebook
* @param session the current facebook session
* @param loginResult the result of the loggin attempt
*/
void Game::FBLoginCallback(struct s3eFBSession* session, s3eResult* loginResult, void* userData)
{
    IwTrace(FACEBOOK, ("FBLoginCallback %p %p %p", session, loginResult, userData));

    if (*loginResult == S3E_RESULT_SUCCESS)
    {
        Game::g_SessionStatus = Constants::FB_LOGGED_IN;
        //sprintf(g_StatusMessage, "`x66ee66Facebook Log-In succeeded");
		//cout << "Facebook Log-In succeeded" << endl;

		//cout << s3eFBSession_AccessToken(g_CurrentSession) << endl;

		if(mHttpObject == NULL) {
			mHttpObject = new CIwHTTP();
		}

		cout << "add user callback" << endl;
		g_GetUID = s3eFBRequest_WithGraphPath(g_CurrentSession, "me");
        s3eResult result = s3eFBRequest_Send(g_GetUID, addUserCallback);
    }
    else
    {
        Game::g_SessionStatus = Constants::FB_LOGGED_OUT;
        //sprintf(g_StatusMessage, "`xee6666Facebook Log-In failed");
		//cout << "Facebook Log-In failed" << endl;
    }

    //UpdateState();

    //g_WaitingForCallback = false;
}

/**
* redirects the player to the facebook login page
*/
void Game::FBLogin() {

	if (!g_CurrentSession)
        g_CurrentSession = s3eFBInit(FACEBOOK_APP_ID);

    if (g_CurrentSession)
    {
		cout << "login attempted" << endl;
        //sprintf(g_StatusMessage, "`x66ee66Session initialised, logging-in...");

        // Log in to Facebook using the session.
        const char* permissions[] = { "read_stream", "publish_stream" };
        s3eFBSession_Login(g_CurrentSession, FBLoginCallback, NULL,
            permissions, sizeof(permissions)/sizeof(permissions[0]));

        Game::g_SessionStatus = Constants::FB_LOGGING_IN;

        //UpdateState();
    }
    else
    {
        //sprintf(g_StatusMessage, "`xee6666Failed to initialise session");
        //g_SessionStatus = FB_UNINITIALISED;
    }

	cout << "login complete" << endl;
}

/**
* logs the player out of facebook, and quits the app
*/
void Game::FBLogout() {

	if(g_SessionStatus == Constants::FB_LOGGED_IN) {
		if(mHttpObject == NULL) {
			mHttpObject = new CIwHTTP;
		}

		//CIwStringL body;
		std::string body = g_UID+",";
		body += g_Token.c_str();
		
		mHttpObject->Post( CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/deleteUserAuth")).c_str(), body.c_str(), body.length(), SignoutCallback, NULL);
	}
	else {
		s3eDeviceExit();
	}
}

/**
* callback, called when the player successfuly logs out of facebook
*/
int32 Game::SignoutCallback(void*, void*) {
	
	//cout << "logout attempted" << endl;
	s3eFBSession_Logout(g_CurrentSession);
    g_SessionStatus = Constants::FB_LOGGED_OUT;
	//cout << "logout complete" << endl;
	s3eDeviceExit();

	return 0;
}

/**
* Callback, called when the player succsessfully logs into facebook and adds them as an authenticated user to the game server
* @param request the facebook request
* @param the request result
*/
void Game::addUserCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData)
{
    IwTrace(FACEBOOK, ("FBGetUIDCallback %p %p %p", request, requestResult, userData));

    if (*requestResult == S3E_RESULT_SUCCESS)
    {
        g_UID = (char*)s3eFBRequest_ResponseDictionaryItemAsString(request, "id");
		//cout << "guid: " << g_UID << endl;

		//CIwStringL playerInfo;
		//playerInfo = CIwStringL(g_UID.c_str()) + CIwStringL(",") + CIwStringL(s3eFBSession_AccessToken(g_CurrentSession));

		//std::string token;
		g_Token = s3eFBSession_AccessToken(g_CurrentSession);

		std::string playerInfo = g_UID+",";// + g_Token.c_str();

		playerInfo += g_Token.c_str();

		if(mHttpObject == NULL) {
			mHttpObject = new CIwHTTP;
		}

		mHttpObject->Post( CIwStringL(CIwStringL(ServerUrl.c_str()) + "/createUserAuth").c_str(), playerInfo.c_str(), playerInfo.length(), NULL, NULL);
    }
    else
    {
        //sprintf(g_StatusMessage, "`xee6666Failed to get user Id");
    }

    s3eFBRequest_Delete(g_GetUID);
    g_GetUID = NULL;

	cout << "request deleted" << endl;
}

/**
* Callback, called when the player finishes a level. Sends their score to the game server if they have logged into facebook
* @param request the facebook request
* @param the request result
*/
void Game::sendScoreCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData)
{
    IwTrace(FACEBOOK, ("FBGetUIDCallback %p %p %p", request, requestResult, userData));

    if (*requestResult == S3E_RESULT_SUCCESS)
    {
        //char* ID = (char*)s3eFBRequest_ResponseDictionaryItemAsString(request, "id");
		g_Score = (1000 - mTimeTaken) - (mPlayerDeaths*50);

		//CIwStringL body;
		//body = CIwStringL(s3eFBSession_AccessToken(g_CurrentSession)) + CIwStringL(",") + CIwStringL(Constants::convertInt(g_Score).c_str()) + CIwStringL(",") + CIwStringL(mLevel->getCurLevel().c_str());
		
		//std::string body = g_UID+",";
		std::string body = g_Token.c_str();
		body += ",";
		body += Constants::convertInt(g_Score).c_str();
		body += ",";
		body += mLevel->getCurLevel().c_str();
		
		mHttpObject->Post( CIwStringL(CIwStringL(ServerUrl.c_str()) + CIwStringL("/sendScoreAuth")).c_str(), body.c_str(), body.length(), NULL, NULL);
		
        //sprintf(g_StatusMessage, "`x66ee66Got user Id: %lld", g_UID);
    }
    else
    {
        //sprintf(g_StatusMessage, "`xee6666Failed to get user Id");
    }

    s3eFBRequest_Delete(g_AddScore);
    g_AddScore = NULL;
}

/**
* Callback, called when the player attempts to post their score to their facebook wall
* @param request the facebook request
* @param the request result
*/
void Game::FBPostRequestCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData)
{
    IwTrace(FACEBOOK, ("FBRequestCallback %p %p %p", request, requestResult, userData));

    if (*requestResult == S3E_RESULT_SUCCESS)
    {
        if (s3eFBRequest_ResponseType(request) == STRING_TYPE)
        {
            cout << "Post succeeded" << endl;
        }
        else
        {
            cout << "Post succeeded" << endl;
        }

		mMenuSystem->hidePostScoreBtn();
    }
    else
    {
		cout << "Post failed" << endl;
	}

    // Clear our single global request and allow reposting
    s3eFBRequest_Delete(PostScoreRequest);
    PostScoreRequest = NULL;
}

/**
* Attempts to post playerscore to their facebook wall
*/
void Game::postScore() {

	PostScoreRequest = s3eFBRequest_WithMethodName(g_CurrentSession, "facebook.stream.publish", "POST");

    if (!PostScoreRequest)
    {
        cout << "Failed to post score" << endl;
    }
    else
    {
        CIwStringL score;

		score = CIwStringL("Just scored ") + CIwStringL(Constants::convertInt(g_Score).c_str()) + CIwStringL(" points in Ascend.");

        // Add a value to the request
		s3eFBRequest_AddParamString(PostScoreRequest, "message", score.c_str());

        // Send the request
        s3eResult result = s3eFBRequest_Send(PostScoreRequest, FBPostRequestCallback);
	}
}

/**
* Gets the currently selected level
* @return the name of the currently selected level
*/
std::string Game::getSelectedLevel() {
	return g_Selectedlvl;
}

/**
* Sets the selected level
* @param the name of the level to select
*/
void Game::setSelectedLevel(std::string pLvlName) {

	g_Selectedlvl = pLvlName;
	g_SelectedLvlScore = g_Scores.find(g_Selectedlvl)->second;
}

bool Game::getStatus() {

	if(Game::g_SessionStatus == Constants::FB_LOGGING_IN) {
		return true;
	}
	else {
		return false;
	}
}

/**
* Pause and resumes the game
*/
void Game::pauseResume() {

	if(mPaused) {
		mPaused = false;
	}
	else {
	
		mPaused = true;
	}
}

/**
* Checks if the game is paused
* @return true if paused, else false
*/
bool Game::isPaused() {
	
	if(mPaused) {
	
		return true;
	}
	else {
	
		return false;
	}
}

/**
* Draws the game world
*/
void Game::drawGame() {

	mPlayer->draw();
	mLevel->drawLevel(CIwSVec2( mPlayer->getBody()->GetPosition().x, mPlayer->getBody()->GetPosition().y));
}

void Game::reloadCheckpoint(b2Vec2 pPos) {

	//mPlayer->loadCheckpoint = true;
	//mPlayer->mPos = CIwSVec2(pPos.x, pPos.y);
	//mCheckpoint = pPos;
}

/**
* Creates an effect (respawn effect, blood, etc.)
* @param pPos the position at which to create the effect
* @param pFrames the number of frames in the effect
* @param pImageName the image to use for the effect
* @param pScale the scale of the effect
*/
void Game::createEffect(CIwSVec2 pPos, int pFrames, const char* pImageName, float pScale) {

	mEffects.push_back(new Effect(pPos, pFrames, pImageName, pScale));
}

/**
* Gets the length of taken to complete a level
* @return the time taken
*/
float Game::getTime() {
	return mTimeTaken;
}

/**
* Gets the player object
* @return the player object
*/
Player* Game::getPlayer() {
	return mPlayer;
}

/**
* Gets other player object in multiplayer
* @return multiplayer partners player object
*/
Player* Game::getOtherPlayer() {
	return mOtherPlayer;
}

/**
* Gets the current level
* @return the current level
*/
Level* Game::getLevel() {
	return mLevel;
}

/**
* Draws Box2D debug information
*/
void Game::b2Debug() {

	// Load the debug images
	CIwImage img;
	img.LoadFromFile("Images/DebugStatic.png");
	CIw2DImage* mImage1 = Iw2DCreateImage(img);
	CIwImage img2;
	img2.LoadFromFile("Images/DebugSleeping.png");
	CIw2DImage* mImage2 = Iw2DCreateImage(img2);
	CIwImage img3;
	img3.LoadFromFile("Images/DebugAwake.png");
	CIw2DImage* mImage3 = Iw2DCreateImage(img3);
	
	int i = 0;
	for (b2Body* b = mLevel->getWorld()->GetBodyList(); b; b = b->GetNext(), i++) {

		CIwSVec2 imageSize((b->GetFixtureList()->GetAABB(0).GetExtents().x*2)*8, (b->GetFixtureList()->GetAABB(0).GetExtents().y*2)*8);
		CIwSVec2 halfImageSize = imageSize >> 1;

		CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

		b2Transform t = b->GetTransform();
		CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));
		float angle = -t.q.GetAngle() * (180.0f/3.14159f);	// reverse angle as our screen-Y coord is reversed

		if(b->GetType() == b2_staticBody) {
			Iw2DDrawImage(mImage1, pos - halfImageSize, imageSize);
		}
		else if(b->IsAwake()){
			Iw2DDrawImage(mImage3, pos - halfImageSize, imageSize);
		}
		else {
			Iw2DDrawImage(mImage2, pos - halfImageSize, imageSize);
		}		
	}
}

/**
* Saves the game to an XML file to loaded later
*/
void Game::saveGame() {

	TiXmlDocument doc;  
 	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl );
	TiXmlElement * root = new TiXmlElement( "document" );  
	doc.LinkEndChild( root );

	mPlayer->save(doc);
	mLevel->save(doc);

	doc.SaveFile( "save.xml" ); 
}

/**
* Destructor
*/
Game::~Game() {

	// Close socket
	//s3eSocketClose(mSock);
	//mSock = NULL;

	mLevel->getWorld()->DestroyBody(mPlayer->getBody());
	b2Body* tempBody;
	for(b2Body* b = mLevel->getWorld()->GetBodyList(); b; ) {
		
		tempBody = b->GetNext();
		mLevel->getWorld()->DestroyBody(b);
		b = tempBody;
	}
	delete mPlayer;		mPlayer = NULL;
	delete mLevel;		mLevel = NULL;
}