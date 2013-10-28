#include "MenuSystem.h"

/**
* Constructor
* @param pWorld the physics world to add the level end sensor to
* @param pPos the position of the level end sensor
* @param pID the id number of the enemy
* @param pDiff the difficulty level of the game
* @param pPatrolA first point which the enemy patrols between
* @param pPatrolB second point the enemy patrols between
*/
MenuSystem::MenuSystem() {

	CIwUIView* view = new CIwUIView();

	//Register an application slot function with the CIwUIView singleton. 
	//Slots registered with the view can be attached in data or code to element signals.	
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Start,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Restart,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_LoadLevel,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_LevelMenu,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_OptionsMenu,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Back,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Quit,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Checkpoint,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_CloseMessage,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_ShowLastMsg,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_QuitToMenu,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Multiplayer,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_HostJoin,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_Pause,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_SetDifficulty,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_RetryLevel,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_FBLogin,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_FBLogout,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_ToMainMenu,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_ToFBLogin,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_SelectLevel,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_StartMultGame,	CIwUIElement*)
	IW_UI_CREATE_VIEW_SLOT1(this, "MenuSystem", MenuSystem, OnClick_PostScore,	CIwUIElement*)

	IwGetResManager()->LoadGroup("textures.group");
	mTexGroup = IwGetResManager()->GetGroupNamed("Textures");

	CIwPropertyDefine* pPropertyDefine = new CIwPropertyDefine("CIwStringS", "name");
	pPropertyDefine->SetDefaultProperty(CIwStringS("none")); // Optional default value
	IwGetResManager()->AddRes(IW_RESTYPE_PROPERTY_DEFINE, pPropertyDefine);

	{
		// Create main menu system
		mainMenu = new CIwUIElement();
		optionsMenu = new CIwUIElement();
		levelMenu = new CIwUIElement();
		deathScreen = new CIwUIElement();
		messageUI = new CIwUIElement();
		lastMessage = new CIwUIElement();
		endScreen = new CIwUIElement();
		pauseBtn = new CIwUIElement();
		createJoinMenu = new CIwUIElement();
		multiLevelMenu = new CIwUIElement();
		difficultyMenu = new CIwUIElement();
		pauseMenu = new CIwUIElement();
		confQuit = new CIwUIElement();
		hostOptions = new CIwUIElement();
		FBLoginMenu = new CIwUIElement();
		FBPostScore = new CIwUIElement();
		retryLevel = new CIwUIElement();
		nextlevel = new CIwUIElement();
		lobbyOptions = new CIwUIElement();

		CIwVec2 menuPos(150, 10);
		mainMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-266, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-266), CIwVec2(256, 256), "start", "OnClick_Start", "startContbtnUp", "startContbtnDown", "Start", mTexGroup));
		mainMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-266, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)+10), CIwVec2(256, 256), "levelMenu", "OnClick_LevelMenu", "levelSelBtnUp", "levelSelBtnDown", "Level Select", mTexGroup));
		//mainMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+152), CIwVec2(300, 66), "optionsMenu", "OnClick_OptionsMenu", "optionsBtnUp", "optionsBtnDown", "Options", mTexGroup));
		mainMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)+10, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-266), CIwVec2(256, 256), "multiplayer", "OnClick_Multiplayer", "multiBtnUp", "multiBtnDown", "Start", mTexGroup));
		mainMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)+10, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)+10), CIwVec2(256, 256), "quit", "OnClick_Quit", "quitGameBtnUp", "quitGameBtnDown", "Quit", mTexGroup));
		mainMenu->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_ToFBLogin", "backBtnUp", "backBtnDown", "none", mTexGroup));
		mainMenu->SetVisible(false);

		FBPostScore->AddChild(createButton(CIwVec2(541, 670), CIwVec2(295, 66), "postScore", "OnClick_PostScore", "postScoreBtnUp", "postScoreBtnDown", "none", mTexGroup));
		FBPostScore->SetVisible(false);

		retryLevel->AddChild(createButton(CIwVec2(541, 573), CIwVec2(295, 66), "nextLevel", "OnClick_LoadLevel", "nextLevelBtnUp", "nextLevelBtnDown", "none", mTexGroup));
		retryLevel->SetVisible(false);
		nextlevel->AddChild(createButton(CIwVec2(187, 670), CIwVec2(295, 66), "retryLevel", "OnClick_RetryLevel", "retryLevelBtnUp", "retryLevelBtnDown", "none", mTexGroup));
		nextlevel->SetVisible(false);
		//difficultyMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y), CIwVec2(300, 66), "easy", "OnClick_SetDifficulty", "startBtnUp", "startBtnDown", "Easy", mTexGroup));
		//difficultyMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+76), CIwVec2(300, 66), "normal", "OnClick_SetDifficulty", "lvlBtnUp", "lvlBtnDown", "Normal", mTexGroup));
		//difficultyMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+152), CIwVec2(300, 66), "hard", "OnClick_SetDifficulty", "optionsBtnUp", "optionsBtnDown", "Hard", mTexGroup));
		//difficultyMenu->SetVisible(false);

		levelMenu->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-256, 10), CIwVec2(512, 128), "lvlSelectTitle", mTexGroup));
		levelMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-266, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-128), CIwVec2(256, 256), "tutorial", "OnClick_SelectLevel", "tutorialBtnUp", "tutorialBtnDown", "none", mTexGroup));
		levelMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)+10, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-128), CIwVec2(256, 256), "level1", "OnClick_SelectLevel", "Level1BtnUp", "Level1BtnDown", "none", mTexGroup));
		levelMenu->AddChild(createButton(CIwVec2(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-160, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "loadLevel", "OnClick_LoadLevel", "startLevelBtnUp", "startLevelBtnDown", "none", mTexGroup));
		levelMenu->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_QuitToMenu", "backBtnUp", "backBtnDown", "none", mTexGroup));
		levelMenu->SetVisible(false);

		multiLevelMenu->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-256, 10), CIwVec2(512, 128), "lvlSelectTitle", mTexGroup));
		multiLevelMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-266, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-128), CIwVec2(256, 256), "test1", "OnClick_SelectLevel", "Level1BtnUp", "Level1BtnDown", "none", mTexGroup));
		multiLevelMenu->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_QuitToMenu", "backBtnUp", "backBtnDown", "none", mTexGroup));
		multiLevelMenu->SetVisible(false);

		optionsMenu->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-256, 10), CIwVec2(512, 128), "optionsTitle", mTexGroup));
		optionsMenu->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_Back", "backBtnUp", "backBtnDown", "none", mTexGroup));
		optionsMenu->SetVisible(false);

		//deathScreen->AddChild(createUIImage(CIwVec2(0, 0), CIwVec2(1024, 768), "deathBackground", mTexGroup));
		//deathScreen->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-60, 50), CIwVec2(120, 156), "deathIcon", mTexGroup));
		//deathScreen->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, 300), CIwVec2(300, 66), "checkPoint", "OnClick_Checkpoint", "checkpointBtnUp", "checkpointBtnDown", "none", mTexGroup));
		//deathScreen->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, 376), CIwVec2(300, 66), "quit", "OnClick_Quit", "quitBtnUp", "quitBtnDown", "Quit", mTexGroup));
		//deathScreen->SetVisible(false);

		//createJoinMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y), CIwVec2(300, 66), "host", "OnClick_HostJoin", "hostBtnUp", "hostBtnUp", "Start", mTexGroup));
		//createJoinMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+76), CIwVec2(300, 66), "join", "OnClick_HostJoin", "joinBtnUp", "joinBtnUp", "Level Select", mTexGroup));
		//createJoinMenu->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_Back", "backBtnUp", "backBtnDown", "none", mTexGroup));
		//createJoinMenu->SetVisible(false);

		messageUI->SetVisible(false);

		lastMessage->AddChild(createButton(CIwVec2(10, 50), CIwVec2(217/4, 371/4), "lastMsg", "OnClick_ShowLastMsg", "helpMsg", "helpMsg", "none", mTexGroup));
		lastMessage->SetVisible(false);

		endScreen->AddChild(createUIImage(CIwVec2(0, 0), CIwVec2(1024, 768), "lvlComplete", mTexGroup));
		endScreen->AddChild(createButton(CIwVec2(187, 573), CIwVec2(295, 66), "mainMenu", "OnClick_QuitToMenu", "mainMenuBtnUp", "mainMenuBtnDown", "none", mTexGroup));
		endScreen->SetVisible(false);

		pauseBtn->AddChild(createButton(CIwVec2(10, 10), CIwVec2(50, 50), "quitBtn", "OnClick_Pause", "menuBtn", "menuBtn", "none", mTexGroup));
		pauseBtn->SetVisible(false);

		pauseMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-266, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-128), CIwVec2(256, 256), "resume", "OnClick_Pause", "resumeBtnUp", "resumeBtnDown", "Resume", mTexGroup));
		pauseMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)+10, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-128), CIwVec2(256, 256), "quitToMenu", "OnClick_QuitToMenu", "quitGameBtnUp", "quitGameBtnDown", "Quit", mTexGroup));
		//pauseMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+152), CIwVec2(300, 66), "saveGame", "OnClick_SaveGame", "optionsBtnUp", "optionsBtnDown", "Save", mTexGroup));
		//pauseMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT))-138, 10), CIwVec2(128, 64), "FBLogout", "OnClick_FBLogout", "fbLogout", "fbLogout", "Quit", mTexGroup));
		pauseMenu->SetVisible(false);

		//confQuit->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y), CIwVec2(300, 66), "yes", "OnClick_ConfQuitGame", "startBtnUp", "startBtnDown", "Yes", mTexGroup));
		//confQuit->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, menuPos.y+76), CIwVec2(300, 66), "no", "OnClick_ConfQuitGame", "lvlBtnUp", "lvlBtnDown", "No", mTexGroup));
		//confQuit->SetVisible(false);

		hostOptions->AddChild(createButton(CIwVec2(s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)-160, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "startMultGame", "OnClick_StartMultGame", "startLevelBtnUp", "startLevelBtnDown", "start", mTexGroup));
		//hostOptions->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_Back", "backBtnUp", "backBtnDown", "none", mTexGroup));
		hostOptions->SetVisible(false);

		lobbyOptions->AddChild(createButton(CIwVec2(10, s3eSurfaceGetInt(S3E_SURFACE_WIDTH)-50), CIwVec2(150, 40), "back", "OnClick_QuitToMenu", "backBtnUp", "backBtnDown", "none", mTexGroup));
		lobbyOptions->SetVisible(false);

		FBLoginMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-256, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)-266), CIwVec2(512, 256), "FBLogin", "OnClick_FBLogin", "fbLogin", "fbLogin", "Resume", mTexGroup));
		FBLoginMenu->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-256, (s3eSurfaceGetInt(S3E_SURFACE_WIDTH)/2)+10), CIwVec2(512, 256), "FBLoginSkip", "OnClick_ToMainMenu", "FBLoginSkip", "FBLoginSkip", "Quit", mTexGroup));
		FBLoginMenu->SetVisible(false);

		view->AddElement(mainMenu);
		view->AddElement(levelMenu);
		view->AddElement(optionsMenu);
		//view->AddElement(createJoinMenu);
		//view->AddElement(deathScreen);
		view->AddElement(messageUI);
		view->AddElement(lastMessage);
		view->AddElement(endScreen);
		view->AddElement(pauseBtn);
		view->AddElement(multiLevelMenu);
		//view->AddElement(difficultyMenu);
		view->AddElement(pauseMenu);
		//view->AddElement(confQuit);
		view->AddElement(hostOptions);
		view->AddElement(FBLoginMenu);
		view->AddElement(FBPostScore);
		view->AddElement(nextlevel);
		view->AddElement(retryLevel);
		view->AddElement(lobbyOptions);

		curPage = FBLoginMenu;
		prevPage = FBLoginMenu;

		curPage->SetVisible(true);
		//FBLoginMenu->SetVisible(true);	
		//FBLoginMenu->GetChildNamed("FBLogout")->SetVisible(false);
	}
}

void MenuSystem::ShowEndLevelScreen() {
	endScreen->SetVisible(true);
	pauseBtn->SetVisible(false);
	updateLoginOptions();

	if(mGame->isMultiplayer()) {

		retryLevel->SetVisible(false);
	}
	else {
		retryLevel->SetVisible(true);
	}

	if(mGame->isHost()) {
		nextlevel->SetVisible(true);
	}
	else {
		nextlevel->SetVisible(false);
	}

	if(mGame->getFBStatus() == Constants::FB_LOGGED_IN) {
		FBPostScore->SetVisible(true);
	}
	else {
		FBPostScore->SetVisible(false);
	}
}

void MenuSystem::showMessage(const char* pMsgID) {

	mGame->pauseResume();
	messageUI->DestroyChildren();
	messageUI->AddChild(createUIImage(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-250, 50), CIwVec2(500, 300), pMsgID, mTexGroup));
	messageUI->AddChild(createButton(CIwVec2((s3eSurfaceGetInt(S3E_SURFACE_HEIGHT)/2)-150, 360), CIwVec2(300, 66), "closeMessage", "OnClick_CloseMessage", "backBtnUp", "backBtnDown", "none", mTexGroup));
	messageUI->SetVisible(true);
	pauseBtn->SetVisible(false);
}

void MenuSystem::init(Game* pGame) {
	mGame = pGame;
	//updateLoginOptions();
}

void MenuSystem::OnClick_StartMultGame(CIwUIElement*) {

	//mGame = pGame;
	//updateLoginOptions();

	CIwStringS lvlName;

	if(mGame->getLevel() == NULL) {
		lvlName = mGame->getSelectedLevel().c_str();

		curPage->SetVisible(false);
		prevPage->SetVisible(false);
		multiLevelMenu->SetVisible(false);
		hostOptions->SetVisible(false);
		lobbyOptions->SetVisible(false);

		mGame->loadMultLevel(lvlName);
		mGame->sendStartMsg();
	}
}

void MenuSystem::OnClick_PostScore(CIwUIElement*) {

	mGame->postScore();
}

void MenuSystem::OnClick_QuitToMenu(CIwUIElement*) {
	
	endScreen->SetVisible(false);
	pauseBtn->SetVisible(false);
	pauseMenu->SetVisible(false);
	levelMenu->SetVisible(false);
	FBPostScore->SetVisible(false);
	retryLevel->SetVisible(false);
	nextlevel->SetVisible(false);
	lobbyOptions->SetVisible(false);
	multiLevelMenu->SetVisible(false);
	mainMenu->SetVisible(true);

	prevPage = mainMenu;
	curPage = mainMenu;

	mGame->destroyLevel();
}

void MenuSystem::OnClick_SelectLevel(CIwUIElement* btn) {

	CIwStringS name;
	btn->GetProperty("name", name, false);

	string lvlName = name.c_str();
	mGame->setSelectedLevel(lvlName);
}

void MenuSystem::OnClick_SetDifficulty(CIwUIElement*) {
	
	
}

void MenuSystem::OnClick_Pause(CIwUIElement*) {
	
	mGame->pauseResume();

	if(pauseMenu->IsVisible() == false) {
		pauseMenu->SetVisible(true);
		updateLoginOptions();
	}
	else {
		pauseMenu->SetVisible(false);
	}
}

void MenuSystem::OnClick_Multiplayer(CIwUIElement*) {
	
	//lobbyOptions->SetVisible(true);

	mGame->findMultiplayerSession();

	prevPage = mainMenu;
	prevPage->SetVisible(false);
	curPage->SetVisible(false);
}

void MenuSystem::OnClick_Start(CIwUIElement*) {
	
	mGame->loadSave();
}

void MenuSystem::OnClick_Restart(CIwUIElement*) {
	
	
}

void MenuSystem::OnClick_LoadLevel(CIwUIElement* btn) {
		
	CIwStringS lvlName;

	if(mGame->getLevel() == NULL) {
		//btn->GetProperty("name", lvlName, false);
		lvlName = mGame->getSelectedLevel().c_str();

		mGame->loadLevel(lvlName);
		curPage->SetVisible(false);
	}
	else {
		
		lvlName = mGame->getLevel()->getNextLevel();

		if(lvlName.c_str() != "none") {

			mGame->loadLevel(lvlName);
			endScreen->SetVisible(false);
			FBPostScore->SetVisible(false);
		}
	}
}

void MenuSystem::OnClick_RetryLevel(CIwUIElement*) {

	if(mGame->getLevel() != NULL) {
		CIwStringS curLevelName = mGame->getLevel()->getCurLevel();

		mGame->destroyLevel();
		mGame->loadLevel(curLevelName);
		endScreen->SetVisible(false);
		FBPostScore->SetVisible(false);
	}
}

void MenuSystem::OnClick_LevelMenu(CIwUIElement*) {
		
	prevPage = curPage;
	curPage = levelMenu;
	prevPage->SetVisible(false);
	curPage->SetVisible(true);

	mGame->setStage(Constants::LEVELSELECT);
	mGame->setSelectedLevel("tutorial");
	mGame->getScores();
}

void MenuSystem::OnClick_OptionsMenu(CIwUIElement*) {
		
	prevPage = curPage;
	curPage = optionsMenu;
	prevPage->SetVisible(false);
	curPage->SetVisible(true);
}

void MenuSystem::OnClick_Back(CIwUIElement*) {
	
	CIwUIElement* tempPage = curPage;
	curPage = prevPage;
	prevPage = tempPage;
	prevPage->SetVisible(false);
	curPage->SetVisible(true);

	hostOptions->SetVisible(false);
	//clientOptions->SetVisible(false);
}

void MenuSystem::OnClick_Quit(CIwUIElement*) {
	
	mGame->FBLogout();
}

void MenuSystem::OnClick_Checkpoint(CIwUIElement*) {
	
}

void MenuSystem::OnClick_CloseMessage(CIwUIElement*) {
	
	messageUI->SetVisible(false);
	pauseBtn->SetVisible(true);
	mGame->pauseResume();
}

void MenuSystem::OnClick_ShowLastMsg(CIwUIElement*) {
	
}

void MenuSystem::OnClick_HostJoin(CIwUIElement*) {
		

}

void MenuSystem::OnClick_FBLogin(CIwUIElement*) {
	
	mGame->FBLogin();

	FBLoginMenu->SetVisible(false);
	mainMenu->SetVisible(true);

	curPage = mainMenu;
	prevPage = mainMenu;

	cout << "main menu visible" << endl;
}

void MenuSystem::OnClick_FBLogout(CIwUIElement* btn) {
	
	mGame->FBLogout();
}

void MenuSystem::OnClick_ToFBLogin(CIwUIElement*) {
	
	mainMenu->SetVisible(false);
	FBLoginMenu->SetVisible(true);

	curPage = FBLoginMenu;
	prevPage = FBLoginMenu;
}

void MenuSystem::OnClick_ToMainMenu(CIwUIElement*) {

	FBLoginMenu->SetVisible(false);
	FBPostScore->SetVisible(false);
	mainMenu->SetVisible(true);

	curPage = mainMenu;
	prevPage = mainMenu;
}

void MenuSystem::SetButtonImages(CIwUIButton* pButton, CIwTexture* pTex1, CIwTexture* pTex2) {

    // Set texture on buttonUp and buttonDown styles of button
    IwUISetPropertyOnInlineStyle(pButton, "buttonUp", "texture", pTex1);
    IwUISetPropertyOnInlineStyle(pButton, "buttonDown", "texture", pTex2);

    // Usually buttonUp and buttonDown are 'border' type drawables. We want a simple image.
    IwUISetPropertyOnInlineStyle(pButton, "buttonUp", "drawableType", IwHashString("image"));
    IwUISetPropertyOnInlineStyle(pButton, "buttonDown", "drawableType", IwHashString("image"));
}


CIwUIButton* MenuSystem::createButton(CIwVec2 pPos, CIwVec2 pSize, CIwStringS pName, const char* pClickSlot, const char* pTexUp, const char* pTexDown, const char* pCaption, CIwResGroup* pTexGroup) {

	CIwTexture* texUp = new CIwTexture();
	texUp = (CIwTexture*)pTexGroup->GetResNamed(pTexUp, "CIwTexture");
	texUp->Upload();
	CIwTexture* texDown = new CIwTexture();
	texDown = (CIwTexture*)pTexGroup->GetResNamed(pTexDown, "CIwTexture");
	texDown->Upload();

	CIwUIButton* btn = new CIwUIButton();
	btn->AttachClickSlot(pClickSlot);
	btn->SetSize(pSize);
	btn->SetPos(pPos);
	/*if(pCaption != "none") {
		btn->SetCaption(pCaption);
	}*/
	btn->SetProperty("name", pName);

	SetButtonImages(btn, texUp, texDown);

	return btn;
}

CIwUIImage* MenuSystem::createUIImage(CIwVec2 pPos, CIwVec2 pSize, const char* pTex, CIwResGroup* pTexGroup) {

	//Create an image
    CIwUIImage* img = new CIwUIImage;
    img->SetProperty("texture", (CIwTexture*)pTexGroup->GetResNamed(pTex, "CIwTexture"));
	img->SetSize(pSize);
	img->SetPos(pPos);

	return img;
}

void MenuSystem::showPauseBtn() {
	pauseBtn->SetVisible(true);
}

void MenuSystem::showLobbyOptions(bool pHost) {

	mGame->setSelectedLevel("test1");

	lobbyOptions->SetVisible(true);

	if(pHost == true) {
		hostOptions->SetVisible(true);
		multiLevelMenu->SetVisible(true);
	}
	else {
		hostOptions->SetVisible(false);
		multiLevelMenu->SetVisible(false);
	}
}

void MenuSystem::updateLoginOptions() {

	/*if(mGame->getStatus() == true) {
		FBLoginMenu->GetChild(0)->SetVisible(false);
		FBLoginMenu->GetChild(1)->SetVisible(true);
	}
	else {
		FBLoginMenu->GetChild(0)->SetVisible(true);
		FBLoginMenu->GetChild(1)->SetVisible(false);
	}*/
}

void MenuSystem::clearMenus() {

	curPage->SetVisible(false);
	prevPage->SetVisible(false);
	multiLevelMenu->SetVisible(false);
	hostOptions->SetVisible(false);
	nextlevel->SetVisible(false);
	retryLevel->SetVisible(false);
	lobbyOptions->SetVisible(false);
}

void MenuSystem::hidePostScoreBtn() {
	FBPostScore->SetVisible(false);
}
