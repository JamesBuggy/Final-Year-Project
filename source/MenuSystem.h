#ifndef MENU_H
#define MENU_H

#include "s3e.h"
#include "Iw2D.h"
#include "IwGx.h"
#include "IwUI.h"
#include "s3eSound.h"
#include "IwUIElement.h"
#include "IwResManager.h"
#include "IwPropertyDefine.h"
#include "Constants.h"
//#include "game.h"

class Game;

class MenuSystem : public CIwUIController
{

public:

    MenuSystem();
	void init(Game* pGame);
	void ShowEndLevelScreen();
	void showPauseBtn();
	void showLobbyOptions(bool pHost);
	void updateLoginOptions();
	void clearMenus();
	void showMessage(const char* pMsgID);
	void hidePostScoreBtn();

private:

	void OnClick_QuitToMenu(CIwUIElement*);
	void OnClick_Start(CIwUIElement*);
	void OnClick_Restart(CIwUIElement*);
	void OnClick_LoadLevel(CIwUIElement* btn);
	void OnClick_SelectLevel(CIwUIElement* btn);
	void OnClick_LevelMenu(CIwUIElement*);
	void OnClick_OptionsMenu(CIwUIElement*);
	void OnClick_Back(CIwUIElement*);
	void OnClick_Quit(CIwUIElement*);
	void OnClick_Checkpoint(CIwUIElement*);
	void OnClick_CloseMessage(CIwUIElement*);
	void OnClick_ShowLastMsg(CIwUIElement*);
	void OnClick_HostJoin(CIwUIElement*);
	void OnClick_Multiplayer(CIwUIElement*);
	void OnClick_Pause(CIwUIElement*);
	void OnClick_SetDifficulty(CIwUIElement*);
	void OnClick_RetryLevel(CIwUIElement*);
	void OnClick_FBLogin(CIwUIElement*);
	void OnClick_FBLogout(CIwUIElement* btn);
	void OnClick_ToMainMenu(CIwUIElement*);
	void OnClick_ToFBLogin(CIwUIElement*);
	void OnClick_StartMultGame(CIwUIElement*);
	void OnClick_PostScore(CIwUIElement*);

	void SetButtonImages(CIwUIButton* pButton, CIwTexture* pTex1, CIwTexture* pTex2);
	CIwUIButton* createButton(CIwVec2 pPos, CIwVec2 pSize, CIwStringS pName, const char* pClickSlot, const char* pTexUp, const char* pTexDown, const char* pCaption, CIwResGroup* pTexGroup);
	CIwUIImage* createUIImage(CIwVec2 pPos, CIwVec2 pSize, const char* pTex, CIwResGroup* pTexGroup);

	Game* mGame;

	CIwResGroup* mTexGroup;

	CIwUIElement* lobbyOptions;
	CIwUIElement* FBPostScore;
	CIwUIElement* nextlevel;
	CIwUIElement* retryLevel;
	CIwUIElement* mainMenu;
	CIwUIElement* optionsMenu;
	CIwUIElement* levelMenu;
	CIwUIElement* multiLevelMenu;
	CIwUIElement* createJoinMenu;
	CIwUIElement* difficultyMenu;
	CIwUIElement* pauseMenu;
	CIwUIElement* confQuit;
	CIwUIElement* FBLoginMenu;

	CIwUIElement* curPage;
	CIwUIElement* prevPage;

	CIwUIElement* deathScreen;
	CIwUIElement* endScreen;

	CIwUIElement* messageUI;
	CIwUIElement* lastMessage;
	CIwStringS lastMessageID;

	CIwUIElement* pauseBtn;
	CIwUIElement* hostOptions;
	CIwUIElement* clientOptions;
	CIw2DImage* menuBackground;
};
#include "game.h"
#endif