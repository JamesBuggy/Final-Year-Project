#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwUI.h"
#include "s3eSound.h"
#include "IwUIElement.h"
#include "IwResManager.h"
#include "IwPropertyDefine.h"

#include <iostream>

#include "Box2D/Box2D.h"
#include "game.h"
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
#include "MenuSystem.h"
#include "IwSound.h"

Game* mGame;
CIw2DImage * moveLeftBtn;
CIw2DImage * moveRightBtn;
CIw2DImage * jumpBtn;
CIw2DImage * kickBtn;
CIw2DImage * punchBtn;

CIwResGroup* mTexGroup;
CIwResGroup* mAudioGroup;
CIw2DFont* mFont;

Constants::GameStage mStage;
CIwStringS selectedLevel;
bool loadingSave;

bool gameEnded;
bool gamePaused;
bool multiplayer;
bool host;
bool sendKickMsg;
bool sendPunchMsg;
int playerDeaths;
b2Vec2 playerCheckpoint;
PacketStream* stream;
int playersFinished;
int Difficulty;
int timeTaken;
int writeSwitch;
int playersJoined;
CIwHTTP *theHttpObject;
char* buffer = NULL;
CIwStringS myIP, theirIP;
int myPort, theirPort;
int64 timeNow;
int64 prevTime;
s3eSocket* mSock;
s3eInetAddress mMy_addr;
s3eInetAddress mDest_addr;
bool playerFound;
CIwStringS ServerUrl;

bool spawnPickup;
b2Vec2 pickupSpawnPoint;
bool spawnCrate;
b2Vec2 crateSpawnPoint;
int crateSpawnType;

//Simple structure to track touches
struct CTouch
{
    int32 x; // position x
    int32 y; // position y
    bool active; // is touch active (currently down)
    int32 id; // touch's unique identifier
};

#define MAX_TOUCHES 10
CTouch g_Touches[MAX_TOUCHES];

/**
* Find an active touch with the specified id, or allocate a free one from the list.
* @param id the id of the desired touch
* @return The touch
*/
CTouch* GetTouch(int32 id)
{
    CTouch* pInActive = NULL;

    for (uint32 i = 0; i < MAX_TOUCHES; i++)
    {
        if (id == g_Touches[i].id)
            return &g_Touches[i];
        if (!g_Touches[i].active)
            pInActive = &g_Touches[i];
    }

    //Return first inactive touch
    if (pInActive)
    {
        pInActive->id = id;
        return pInActive;
    }

    //No more touches, give up.
    return NULL;
}

/**
* Touch event callback. Called when the user touches the screen
* @param event the touch event
*/
void MultiTouchButtonCB(s3ePointerTouchEvent* event)
{
	if(mGame->getLevel() != NULL) {
		CTouch* touch = GetTouch(event->m_TouchID);
		if (touch)
		{
			touch->active = event->m_Pressed != 0;
			touch->x = event->m_x;
			touch->y = event->m_y;

			if(!gamePaused) {
				// Left button
				if(touch->x > 110 && touch->x < 220 && touch->y > 10 && touch->y < 110) {

					touch->active ? mGame->getPlayer()->setDir(-1) : mGame->getPlayer()->setDir(0);
				}
				// Right button
				else if(touch->x > 10 && touch->x < 110 && touch->y > 120 && touch->y < 220) {

					touch->active ? mGame->getPlayer()->setDir(1) : mGame->getPlayer()->setDir(0);
				}

				// Jump button
				if(touch->x > 110 && touch->x < 220 && touch->y > s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-110 && touch->y < s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-10) {

					if(touch->active) {
						mGame->getPlayer()->jump();
					}
				}
				// Punch button
				else if(touch->x > 110 && touch->x < 220 && touch->y > s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-220 && touch->y < s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-120) {

					if(touch->active) {

						// Play punch animation
						mGame->getPlayer()->punch();

						vector<Enemy*> enemies = mGame->getLevel()->getEnemies();
						b2Body* player = mGame->getPlayer()->getBody();

						for(int i = 0; i < enemies.size(); i++) {				
							if( abs((enemies.at(i)->getBody()->GetPosition() - player->GetPosition()).Length()) <= 10) {
								short direction = 1;

								if(enemies.at(i)->getBody()->GetPosition().x < player->GetPosition().x) {
									direction = -1;
								}

								b2Vec2 force = b2Vec2(50000*direction, 0);
								enemies.at(i)->getBody()->ApplyForce(force, enemies.at(i)->getBody()->GetWorldCenter());
								enemies.at(i)->ApplyDamage(mGame->getPlayer()->getMeleeStr());

								CIwSVec2 screenCentre = CIwSVec2((int16)Iw2DGetSurfaceWidth() >> 1, (int16)Iw2DGetSurfaceHeight() >> 1);

								b2Transform t = enemies.at(i)->getBody()->GetTransform();
								CIwSVec2 pos = screenCentre + (CIwSVec2(int16(t.p.x*8), -int16(t.p.y*8)));

								mGame->createEffect(pos, 8 , "Images/blood.png", 1.0f);
							}
						}

						//sendPunchMsg = true;
						mGame->create(Constants::ACTION, 0, 0, 0);
					}
				}
				// Kick button
				else if(touch->x > 10 && touch->x < 110 && touch->y > s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-220 && touch->y < s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-120) {

					if(touch->active) {
					
						// Play kick animation
						mGame->getPlayer()->kick();

						vector<Enemy*> enemies = mGame->getLevel()->getEnemies();
						b2Body* player = mGame->getPlayer()->getBody();

						for(int i = 0; i < enemies.size(); i++) {				
							if( abs((enemies.at(i)->getBody()->GetPosition() - player->GetPosition()).Length()) <= 10) {
								short direction = 1;

								if(enemies.at(i)->getBody()->GetPosition().x < player->GetPosition().x) {
									direction = -1;
								}

								b2Vec2 force = b2Vec2(500000*direction, 600000);
								enemies.at(i)->getBody()->ApplyForce(force, enemies.at(i)->getBody()->GetWorldCenter());
								enemies.at(i)->stun();
							}
						}

						vector<Switch*> switches = mGame->getLevel()->getSwitches();

						for(int i = 0; i < switches.size(); i++) {				
							if( abs((switches.at(i)->getPosition() - player->GetPosition()).Length()) <= 10) {
								
								switches.at(i)->toggle();

								mGame->toggleSwitch(i);
							}
						}

						vector<ActivatablePlatform*> platforms = mGame->getLevel()->getActivatablePlatforms();

						for(int i = 0; i < platforms.size(); i++) {		
							if( abs((platforms.at(i)->getSwitch()->GetPosition() - player->GetPosition()).Length()) <= 10) {
							
								//if(host) {
								platforms.at(i)->toggle();
	
								mGame->create(Constants::TOGGLEPLATFORM, i, 0, 0);
								//}
							}
						}

						vector<CrateSpawner*> spawners = mGame->getLevel()->getCrateSpawners();

						for(int i = 0; i < spawners.size(); i++) {				
							if( abs((spawners.at(i)->getSwitch()->GetPosition() - player->GetPosition()).Length()) <= 10) {
								spawners.at(i)->toggleSwitch();

								// Create Crate
								int type = 0;
								if(spawners.at(i)->getType() == "woodenCrateSpawner") {
									
									type = 1;
								}
								else if(spawners.at(i)->getType() == "metalCrateSpawner") {
									
									type = 2;
								}

								//if(host) {
									//mGame->getLevel()->createCrate(1, spawners.at(i)->getSpawner()->GetPosition().x, spawners.at(i)->getSpawner()->GetPosition().y);
									mGame->create(Constants::CREATECRATE, type, 33.5, 17);
								//}
								//spawnCrate = true;
								//crateSpawnPoint = spawners.at(i)->getSpawner()->GetPosition();
								//crateSpawnType = type;

								// Send create message
								//PacketStream s;
								//s.writeInt(Constants::CREATECRATE);
								//s.writeInt(mGame->getLevel()->getCrates().back()->getID());
								//s.writeInt(type);
								//s.writeFloat(26.5);
								//s.writeFloat(22);
								//s.writeChar('\0');

								/*char message[10];
								s.toCharArray(message);
								message[strlen(message)-1] = '\0';

								s3eSocketSendTo(mSock, message, strlen(message), 0, &mDest_addr);
								*/
							}
						}
					
						//sendKickMsg = true;
						mGame->create(Constants::ACTION, 1, 0, 0);
					}
				}
			}
		}
	}
}

/**
* The entry point of the application
*/
int main()
{
	Iw2DInit();
	IwUIInit();
	IwGxInit();
	IwSoundInit();
	IwResManagerInit();

	IwGxSetScreenOrient(IW_GX_ORIENT_90);

	#ifdef IW_BUILD_RESOURCES
		// Tell resource system how to convert WAV files
		IwGetResManager()->AddHandler(new CIwResHandlerWAV);
	#endif

	IwGetResManager()->LoadGroup("sounds.group");

	mGame = new Game();

	// Check if device supports multi touch events
	bool g_UseMultiTouch = s3ePointerGetInt(S3E_POINTER_MULTI_TOUCH_AVAILABLE) ? true : false;
    if (g_UseMultiTouch)
    {
        //Register for multi touch events on platforms where the functionality is available.
        s3ePointerRegister(S3E_POINTER_TOUCH_EVENT, (s3eCallback)MultiTouchButtonCB, NULL);
    }

    // Loop forever, until the user or the OS performs some action to quit the app
    while(!s3eDeviceCheckQuitRequest()
          && !(s3eKeyboardGetState(s3eKeyEsc) & S3E_KEY_STATE_DOWN)
          && !(s3eKeyboardGetState(s3eKeyAbsBSK) & S3E_KEY_STATE_DOWN)
          ) 
	{
		stream = new PacketStream();
		
		Iw2DSurfaceClear(0xE3E3E3);
		mGame->update(stream);

		// Update Iw Sound Manager
		IwGetSoundManager()->Update();

		Iw2DSetTransformMatrix(CIwMat2D::g_Identity);

		if(mGame->mStage != Constants::ENDLEVEL) {
			IwGxFlush();
		}

		IwGetUIView()->Render();
		IwGetUIController()->Update();

		Iw2DSurfaceShow();

		//IwGxFlush();
		//IwGxSwapBuffers();

		delete stream;
		stream = NULL;

        // Sleep for 0ms to allow the OS to process events etc.
        s3eDeviceYield(0);
    }

	IwGetUIView()->DestroyElements();
	delete mGame;		mGame = NULL;

	Iw2DTerminate();
	IwUITerminate();
	IwSoundTerminate();

    // Return
    return 0;
}







