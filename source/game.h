#define MAX_TOUCHES 10
#ifndef GAME_H
#define GAME_H

#include "s3eSocket.h"
#include "player.h"
#include "Enemy.h"
#include "level.h"
#include "Effect.h"
#include <vector>
#include "PacketStream.h"
#include "Box2D\Box2D.h"
#include "tinyxml.h"
#include "IwHTTP.h"
#include <iostream>
#include "MenuSystem.h"
#include "CollisionManager.h"
#include "s3eFacebook.h"
#include <map>
#include <ctime>
#include "Enemy.h"
#include "s3eSound.h"

class Game {

	static const int bufferSize = 128;
	static int32 GotGetData(void*, void*);
	static int32 GotPostData(void*, void*);
	static int32 GotScoreData(void*, void*);
	static int32 SignoutCallback(void*, void*);
	static char* mRequestBuffer;
	static bool mHost;
	static s3eSocket* mSock;
	static s3eInetAddress mMy_addr;
	static s3eInetAddress mDest_addr;
	static int mPlayersInLobby;
	static bool mPlayerFound;
	static CIwHTTP* mHttpObject;
	static CIwStringS ServerUrl;
	static string g_UID;
	static s3eFBRequest* g_GetUID;
	static s3eFBRequest* g_AddScore;
	static s3eFBRequest* PostScoreRequest;
	static Level* mLevel;
	static std::map<std::string, int> g_Scores;
	static int g_SelectedLvlScore;
	static std::string g_Selectedlvl;
	static MenuSystem* mMenuSystem;
	static int g_Score;
	
	static float mTimeTaken;
	static int mPlayerDeaths;

	static std::string g_Token;
	
	static int maxCrates;

	static s3eFBSession* g_CurrentSession;
	static Constants::s3eFBStatus g_SessionStatus;
	static void FBLoginCallback(struct s3eFBSession* session, s3eResult* loginResult, void* userData);
	static void addUserCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData);
	static void sendScoreCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData);
	static void FBPostRequestCallback(struct s3eFBRequest* request, s3eResult* requestResult, void* userData);

public:

	static bool crateSpawned;
	static Constants::GameStage mStage;

	Game();
	~Game();

	Level* getLevel();
	Player* getPlayer();
	Player* getOtherPlayer();

	void loadLevel(CIwStringS pLevel);
	void loadMultLevel(CIwStringS pLvl);
	void loadSave();
	void update(PacketStream* pStream);
	void drawGame();
	void createEffect(CIwSVec2 pPos, int pFrames, const char* pImageName, float pScale);
	void reloadCheckpoint(b2Vec2 pPos);
	void b2Debug();
	void pauseResume();
	bool isPaused();
	void saveGame();
	float getTime();
	b2Vec2 getCheckPoint();
	void setCheckPoint(b2Vec2 pCheckpoint);
	bool isMultiplayer();
	void setPickupSpawnPoint(b2Vec2 pPos);
	void spawnPickup(bool pVal);
	void findMultiplayerSession();
	void FBLogin();
	void FBLogout();
	bool getStatus();
	void setStage(Constants::GameStage pStage);
	void getScores();
	void setSelectedLevel(std::string pLvlName);
	std::string getSelectedLevel();
	void sendStartMsg();
	void postScore();
	void create(int pMsgType, int pObjType, float xPos, float yPos);
	void toggleSwitch(int pID);
	Constants::s3eFBStatus getFBStatus();
	bool isHost();
	void showMessage(const char* pMsgID);

	void endLevel();
	void destroyLevel();

	bool loadCheckpoint;
	int timeSinceLastSend;

private:

	void lobbyUpdate(PacketStream* pStream, float pDeltaTime);
	
	const char* FACEBOOK_APP_ID;// = "160040267492274";
	const char* FACEBOOK_APP_SECRET;// = "b7d69cf0ef9e50a344d892df3c8c7fe6";

	MyContactListener* mCollListener;

	Player* mPlayer;
	Player* mDRPlayer;
	Player* mOtherPlayer;

	int mRDXVel;
	int mRecRDXVel;
	
	vector<Effect*> mEffects;
	bool mPaused;
	bool mMultiplayer;

	b2Vec2 mPickupSpawnPoint;
	int mPickupSpawnType;
	bool mSpawnPickup;
	
	CIwStringS mSelectedLevel;
	float mTimeSinceLastRequest;

	char mMessageReceived[bufferSize];
	char mMessageSent[bufferSize];
	int mBytesRecieved;
	char logBuffer[bufferSize];

	const int		physicsHz;
	float			timeStep;
	int				velocityIterations;
	int				positionIterations;
	int64			g_prevTime, g_timeNow;
	float			g_deltaTime, g_accumulator;
	
	b2Vec2 mCheckpoint;

	int mDiff;
	int mPlayersFinished;
	CIw2DFont* mFont;

	// UI Buttons
	CIw2DImage * moveLeftBtn;
	CIw2DImage * moveRightBtn;
	CIw2DImage * jumpBtn;
	CIw2DImage * kickBtn;
	CIw2DImage * punchBtn;

	vector<string> mMusic;
};

#endif