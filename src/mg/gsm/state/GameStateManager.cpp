/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameStateManager.cpp

	See GameStateManager.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\graphics\RenderList.h"
#include "mg\graphics\TextureManager.h"
#include "mg\gsm\state\GameState.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGLevelImporter.h"
#include "mg\text\TextGenerator.h"
#include "rebelle\RebelleTextGenerator.h"

#include "mg\gsm\sprite\Upgrade.h"

/*
	GameStateManager - Default Constructor, it starts the app at the
	splash screen with no level loaded.
*/
GameStateManager::GameStateManager()	
{
}

/*
	~GameStateManager - This destructor should inform the game world and
	sprite managers to clean up all their allocated memory.
*/
GameStateManager::~GameStateManager()
{
}

void GameStateManager::startUp()
{
	spriteManager = new SpriteManager();
	currentGameState = GS_SPLASH_SCREEN;
	currentLevelIndex = NO_LEVEL_LOADED;

	this->setDialoguestart(true);
	/*this->dialoguestart.push_back(LEVEL1START1);
	this->dialoguestart.push_back(LEVEL1START2);
	this->dialoguestart.push_back(LEVEL1START3);
	this->dialoguestart.push_back(LEVEL1START4);
	this->dialoguestart.push_back(LEVEL1START5);
	this->dialoguestart.push_back(LEVEL1START6);
	this->dialoguestart.push_back(LEVEL1START7);
	this->dialoguestart.push_back(LEVEL1START8);
	this->dialoguestart.push_back(LEVEL1START9);
	this->dialoguestart.push_back(LEVEL1START10);
	this->dialoguestart.push_back(LEVEL1START11);
	this->dialoguestart.push_back(LEVEL1START12);
	this->dialoguestart.push_back(LEVEL1START13);
	this->dialoguestart.push_back(LEVEL1START14);
	this->dialoguestart.push_back(LEVEL1START15);*/
}

/*
	addGameRenderItemsToRenderList - This method adds all the world and sprite
	items in the viewport to the render list
*/
void GameStateManager::addGameRenderItemsToRenderList()
{
	// FIRST THE STATIC WORL
	world.addWorldRenderItemsToRenderList();

	// THEN THE SPRITE MANAGER
	spriteManager->addSpriteItemsToRenderList();
}


/*
	goToGame - This method transitions the game application from the levl loading
	to the actualy game.
*/
void GameStateManager::goToGame()
{
	currentGameState = GS_GAME_IN_PROGRESS;
}

/*
	goToLoadLevel - This method transitions the game application from main menu
	to the level loading.
*/
void GameStateManager::goToLoadLevel()
{
	currentGameState = GS_LOADING_LEVEL;
}

/*
	goToMainMenu - This method transitions the game application from the splash
	screen to the main menu.
*/
void GameStateManager::goToMainMenu()
{
	currentGameState = GS_MAIN_MENU;
	currentLevelIndex = NO_LEVEL_LOADED;
	this->unloadCurrentLevel();

	this->score = 0;
	this->money = 0;
}

void GameStateManager::goToPreGame()
{
	currentGameState = GS_PRE_GAME;
}

//// -- paused menu 
void GameStateManager::goToIngamePauseMenu()
{
	currentGameState = GS_PAUSED;
}

//// --- upgrade screen
void GameStateManager::goToUpgradeScreen()
{
	currentGameState = GS_UPGRADE_SCREEN;
}

void GameStateManager::goToDonateScreen()
{
	currentGameState = GS_DONATE_SCREEN;
}

//// go to level complete
void GameStateManager::goToLevelComplete()
{
	currentGameState = GS_LEVEL_COMPLETE;
}

//// GO TO credits screen
void GameStateManager::goToCreditsScreen()
{
	currentGameState = GS_CREDITS_SCREEN;
}

void GameStateManager::goToGameOverScreen()
{
	currentGameState = GS_GAME_OVER;
}

//// go to controls menu
void GameStateManager::goToControlsMenu() {
	currentGameState = GS_MENU_CONTROLS_MENU;
}

/*
	isAtSplashScreen - Used to test if this application is currently
	at the splash screen. This will dictate what to render, but also
	how to respond to user input.
*/
bool GameStateManager::isAtSplashScreen()
{
	return currentGameState == GS_SPLASH_SCREEN;
}

/*
	isAppActive - Used to test if this application is going to continue
	running for another frame. This will return true if the game app is
	not closing, false otherwise. Note that when the currentGameState
	becomes GS_EXIT_GAME, this will return true, and the game loop will
	end.
*/
bool GameStateManager::isAppActive()
{
	return currentGameState != GS_EXIT_GAME;
}

/*
	isGameInProgress - Used to test if the game is running right now, which
	means we have to execute all game logic.
*/
bool GameStateManager::isGameInProgress()
{
	return currentGameState == GS_GAME_IN_PROGRESS;
}

/*
	isGameLevelLoading - Used to test if the game is loading a level right now.
*/
bool GameStateManager::isGameLevelLoading()
{
	return currentGameState == GS_LOADING_LEVEL;
}

bool GameStateManager::isPreGame()
{
	return currentGameState == GS_PRE_GAME;
}

bool GameStateManager::isGameAtUpgradeScreen()
{
	return currentGameState == GS_UPGRADE_SCREEN;
}

bool GameStateManager::isGameAtDonateScreen()
{
	return currentGameState == GS_DONATE_SCREEN;
}

/*
	isWorldRenderable - Used to test if the game world should be rendered
	or not. Note that even if the game is paused, you'll likely still render
	the game.
*/
bool GameStateManager::isWorldRenderable()
{
	return (	(currentGameState == GS_GAME_IN_PROGRESS)
		||		(currentGameState == GS_PAUSED)
		||		(currentGameState == GS_GAME_OVER)
		||		(currentGameState == GS_UPGRADE_SCREEN)
		||		(currentGameState == GS_DONATE_SCREEN)
		||		(currentGameState == GS_LEVEL_COMPLETE) );
	//// GS_UPGRADE_SCREEN added --- by Bongsung 23 APR
	//// GS_LEVEL_COMPLETE added --- by Bongsung 03 MAY
}

bool GameStateManager::isGameInPauseMenu()
{
	return currentGameState == GS_PAUSED;
}

/*
	addLevelFileName - This method adds a level file name to the vector
	of all the level file names. Storing these file names allows us to
	easily load a desired level at any time.
*/
void GameStateManager::addLevel(wstring levelToAddName, wstring levelToAddDir, wstring levelToAddFile)
{
	levelNames.push_back(levelToAddName);
	levelDirs.push_back(levelToAddDir);
	levelFiles.push_back(levelToAddFile);
}

/*
	getLevelNum - Note that when our game app starts, we load all the level
	file names and this should never change. This method looks through those
	names and gets the index in the vector for a given level name. Note that
	we're using a little iterator/pointer arithmetic here since vectors
	guarantee they will store their contents in a continuous block of memory.
*/
unsigned int GameStateManager::getLevelNum(wstring levelName)
{
	return find(levelNames.begin(),levelNames.end(), levelName) - levelNames.begin();
}

/*
	loadLevel - This method changes the current level. This method should
	be called before loading all the data from a level file.
*/
void GameStateManager::loadLevel(unsigned int initLevel)
{
	Game *game = Game::getSingleton();
	if ((initLevel != NO_LEVEL_LOADED) && (initLevel < levelNames.size()))
	{
		if (currentLevelIndex != NO_LEVEL_LOADED)
			unloadCurrentLevel();
		currentLevelIndex = initLevel;
		wstring fileDirToLoad = levelDirs[currentLevelIndex];
		wstring fileToLoad = levelFiles[currentLevelIndex];
		GameResources *resources = game->getResources();
		MGImporter *levelImporter = resources->getLevelImporter();
		levelImporter->load(fileDirToLoad, fileToLoad);
		int i = 0;
	}
}

/*
	loadLevel - This is just an overladed version of our loadLevel method that
	uses the name of the level rather than its index. Some games may have non-
	linear levels, and so the developers may prefer to hold onto the level names
	rather than numbers.
*/
void GameStateManager::loadLevel(wstring levelName)
{
	unsigned int levelIndex = getLevelNum(levelName);
	loadLevel(levelIndex);
}

void GameStateManager::loadCurrentLevel()
{
	if (currentLevelIndex != NO_LEVEL_LOADED)
	{
		loadLevel(currentLevelIndex);

		//// set viewport to the initial player location 
		Game *game = Game::getSingleton();
		GameGUI *gui = game->getGUI();
		Viewport *viewport = gui->getViewport();

		PlayerSprite *player = spriteManager->getPlayer();
		PhysicalProperties *playerPP = player->getPhysicalProperties();
		int playerSpriteWidth = player->getSpriteType()->getTextureWidth();
		int playerSpriteHeight = player->getSpriteType()->getTextureHeight();
		int playerCenterX = playerPP->getX() + playerSpriteWidth / 2;
		int playerCenterY = playerPP->getY() + playerSpriteHeight / 2;

		int viewportLeft = playerCenterX - viewport->getViewportWidth() / 2;
		int viewportTop = playerCenterY - viewport->getViewportHeight() / 2;

		int viewportRight = playerCenterX + viewport->getViewportWidth() / 2;
		int viewportBottom = playerCenterY + viewport->getViewportHeight() / 2;

		int viewportX = 0;
		int viewportY = 0;

		World *world = game->getGSM()->getWorld();

		if (viewportLeft < 0)
			viewportX = 0;
		if (viewportRight > world->getWorldWidth())
			viewportX = world->getWorldWidth() - viewport->getViewportWidth();
		if (viewportTop < 0)
			viewportY = 0;
		if (viewportBottom > world->getWorldHeight())
			viewportY = world->getWorldHeight() - viewport->getViewportHeight();

		/// now, set the viewport to the new level's initial location
		viewport->setViewportX(viewportX);
		viewport->setViewportY(viewportY);
	}
}

void GameStateManager::loadNextLevel()
{
	int newkey = rand() % 9;
	setSafety(newkey);
	if (currentLevelIndex == NO_LEVEL_LOADED)
	{
		currentLevelIndex = 0;
	}
	else if (currentLevelIndex < levelFiles.size())
	{
		currentLevelIndex++;
	}
	loadCurrentLevel();
}

/*
	shutdown - this method is called when the user wants to quit the
	application. This method updates the game state such that all
	world resources are released and the game loop does not iterate
	again.
*/
void GameStateManager::shutDown()
{
	// MAKE SURE THE GAME LOOP DOESN'T GO AROUND AGAIN
	currentGameState = GS_EXIT_GAME;
}

/*
	unloadCurrentLevel - This method removes all data from the World, recovering
	all used memory. It should be called first when a level is unloaded or changed. 
	If it is not called, an application runs the risk of having memory leaking,
	i.e. extra data sitting around that may slow the progam down. Or, if the app
	thinks a level is still active, it might add items to the render list using 
	image ids that have already been cleared from the GameGraphics' texture manager 
	for the world. That would likely result in an exception.
*/
void GameStateManager::unloadCurrentLevel()
{
	spriteManager->unloadSprites();
	world.unloadWorld();
}

/*
	update - This method should be called once per frame. It updates
	both the sprites and the game world. Note that even though the game
	world is for static data, should the user wish to put anything dynamic
	there (like a non-collidable moving layer), the updateWorld method
	is called.
*/
void GameStateManager::update()
{
	Game *game = Game::getSingleton();
	GameClock *clock = game->getClock();
	GameText *text = game->getText();
	TextGenerator *generator = text->getTextGenerator();
	gameStateMachine->update();
	spriteManager->update();
	world.update();

	int newkey = getSafety();

	//generator->setTime(countdownCounter);
	//if (countdownCounter/40 <= 5 && countdownCounter/40 > 0)
	generator->setTime(countdownCounter);

	if (countdownCounter <= 1)
	{
		while (newkey == getSafety())
		{
			newkey = rand() % 9;
		}
		setSafety(newkey);
		countdownCounter = 500;
	}
	countdownCounter--;

	if (this->getDialoguestart() == true)
	{
		//this->getPhysics()->togglePlayerPhysics();

		switch (this->getDialoguestartindex())
		{

		case(0):
			this->setDialoguetext(LEVEL1START1);
			break;
		case(1):
			this->setDialoguetext(LEVEL1START2);
			break;
		case(2):
			this->setDialoguetext(LEVEL1START3);
			break;
		case(3):
			this->setDialoguetext(LEVEL1START4);
			break;
		case(4):
			this->setDialoguetext(LEVEL1START5);
			break;
		case(5):
			this->setDialoguetext(LEVEL1START6);
			break;
		case(6):
			this->setDialoguetext(LEVEL1START7);
			break;
		case(7):
			this->setDialoguetext(LEVEL1START8);
			break;
		case(8):
			this->setDialoguetext(LEVEL1START9);
			break;
		case(9):
			this->setDialoguetext(LEVEL1START10);
			break;
		case(10):
			this->setDialoguetext(LEVEL1START11);
			break;
		case(11):
			this->setDialoguetext(LEVEL1START12);
			break;
		case(12):
			this->setDialoguetext(LEVEL1START13);
			break;
		case(13):
			this->setDialoguetext(LEVEL1START14);
			break;
		case(14):
			this->setDialoguetext(LEVEL1START15);
			break;
		default:
			this->setDialoguetext(L"");
			this->setDialoguestart(false);
			//this->getPhysics()->togglePlayerPhysics();
			break;
		}
	}
	else if (this->getDialogueenemy() == true)
	{
		//this->getPhysics()->togglePlayerPhysics();

		switch (this->getDialogueenemyindex())
		{

		case(0):
			this->setDialoguetext(LEVEL1FOUNDENEMY1);
			break;
		case(1):
			this->setDialoguetext(LEVEL1FOUNDENEMY2);
			break;
		case(2):
			this->setDialoguetext(LEVEL1FOUNDENEMY3);
			break;
		case(3):
			this->setDialoguetext(LEVEL1FOUNDENEMY4);
			break;
		case(4):
			this->setDialoguetext(LEVEL1FOUNDENEMY5);
			break;
		case(5):
			this->setDialoguetext(LEVEL1FOUNDENEMY6);
			break;
		case(6):
			this->setDialoguetext(LEVEL1FOUNDENEMY7);
			break;
		case(7):
			this->setDialoguetext(LEVEL1FOUNDENEMY8);
			break;
		case(8):
			this->setDialoguetext(LEVEL1FOUNDENEMY9);
			break;
		case(9):
			this->setDialoguetext(LEVEL1FOUNDENEMY10);
			break;
		case(10):
			this->setDialoguetext(LEVEL1FOUNDENEMY11);
			break;
		case(11):
			this->setDialoguetext(LEVEL1FOUNDENEMY12);
			break;
		case(12):
			this->setDialoguetext(LEVEL1FOUNDENEMY13);
			break;
		case(13):
			this->setDialoguetext(LEVEL1FOUNDENEMY14);
			break;
		case(14):
			this->setDialoguetext(LEVEL1FOUNDENEMY15);
			break;
		default:
			this->setDialoguetext(L"");
			this->setDialogueenemy(false);
			this->getPhysics()->togglePhysics();
			break;
		}
	}
	else if (this->getDialoguemoney() == true)
	{
		//this->getPhysics()->togglePlayerPhysics();

		switch (this->getDialoguemoneyindex())
		{

		case(0):
			this->setDialoguetext(LEVEL1MONEY1);
			break;
		case(1):
			this->setDialoguetext(LEVEL1MONEY2);
			break;
		case(2):
			this->setDialoguetext(LEVEL1MONEY3);
			break;
		case(3):
			this->setDialoguetext(LEVEL1MONEY4);
			break;
		case(4):
			this->setDialoguetext(LEVEL1MONEY5);
			break;
		case(5):
			this->setDialoguetext(LEVEL1MONEY6);
			break;
		case(6):
			this->setDialoguetext(LEVEL1MONEY7);
			break;
		case(7):
			this->setDialoguetext(LEVEL1MONEY8);
			break;
		case(8):
			this->setDialoguetext(LEVEL1MONEY9);
			break;
		case(9):
			this->setDialoguetext(LEVEL1MONEY10);
			break;
		case(10):
			this->setDialoguetext(LEVEL1MONEY11);
			break;
		case(11):
			this->setDialoguetext(LEVEL1MONEY12);
			break;
		case(12):
			this->setDialoguetext(LEVEL1MONEY13);
			break;
		case(13):
			this->setDialoguetext(LEVEL1MONEY14);
			break;
		case(14):
			this->setDialoguetext(LEVEL1MONEY15);
			break;
		default:
			this->setDialoguetext(L"");
			this->setDialoguemoney(false);
			this->getPhysics()->togglePhysics();
			break;
		}
	}
	else if (this->getDialogueheal() == true)
	{
		//this->getPhysics()->togglePlayerPhysics();

		switch (this->getDialoguehealindex())
		{

		case(0):
			this->setDialoguetext(LEVEL1HEAL1);
			break;
		case(1):
			this->setDialoguetext(LEVEL1HEAL2);
			break;
		case(2):
			this->setDialoguetext(LEVEL1HEAL3);
			break;
		case(3):
			this->setDialoguetext(LEVEL1HEAL4);
			break;
		case(4):
			this->setDialoguetext(LEVEL1HEAL5);
			break;
		case(5):
			this->setDialoguetext(LEVEL1HEAL6);
			break;
		case(6):
			this->setDialoguetext(LEVEL1HEAL7);
			break;
		case(7):
			this->setDialoguetext(LEVEL1HEAL8);
			break;
		case(8):
			this->setDialoguetext(LEVEL1HEAL9);
			break;
		default:
			this->setDialoguetext(L"");
			this->setDialogueheal(false);
			this->getPhysics()->togglePhysics();
			break;
		}
	}

	if (this->getDialogueenemyindex() > 14 && this->getDialoguemoneyindex() > 14 && this->getDialoguehealindex() > 8)
	{
		if (dialoguecounter == 7400 && innerindex < 2)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 0)
				this->setDialoguetext(MISCA1);
			else if (innerindex == 1)
				this->setDialoguetext(MISCA2);

		}
		else if (dialoguecounter == 6800 && innerindex < 3)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 2)
				this->setDialoguetext(MISCB1);

		}
		else if (dialoguecounter == 6200 && innerindex < 5)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 3)
				this->setDialoguetext(MISCC1);
			else if (innerindex == 4)
				this->setDialoguetext(MISCC2);
		}
		else if (dialoguecounter == 5600 && innerindex < 8)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 5)
				this->setDialoguetext(MISCD1);
			else if (innerindex == 6)
				this->setDialoguetext(MISCD2);
			else if (innerindex == 7)
				this->setDialoguetext(MISCD3);
		}
		else if (dialoguecounter == 5000 && innerindex < 12)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 8)
				this->setDialoguetext(MISCE1);
			else if (innerindex == 9)
				this->setDialoguetext(MISCE2);
			else if (innerindex == 10)
				this->setDialoguetext(MISCE3);
			else if (innerindex == 11)
				this->setDialoguetext(MISCE4);
		}
		else if (dialoguecounter == 4400 && innerindex < 14)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 12)
				this->setDialoguetext(MISCF1);
			else if (innerindex == 13)
				this->setDialoguetext(MISCF2);
		}
		else if (dialoguecounter == 3800 && innerindex < 15)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 14)
				this->setDialoguetext(MISCG1);
		}
		else if (dialoguecounter == 3200 && innerindex < 16)
		{
			innerdialoguecounter--;
			if (innerdialoguecounter == 0)
			{
				innerindex++;
				innerdialoguecounter = 300;
			}

			if (innerindex == 15)
				this->setDialoguetext(MISCH1);
		}
		else if (dialoguecounter == 2600)
		{
			dialoguecounter = 8000;
		}
		else
		{
			this->setDialoguetext(L"");
			dialoguecounter--;
		}
	}


	physics.update();
}

void GameStateManager::addUpgrade(wstring type) 
{ 
	Upgrade *u = new Upgrade();

	u->setType(type);
	if (type.compare(L"ATTACK") == 0)
		setAttackupgrades(getAttackupgrades() + 1);
	else if (type.compare(L"DEFENSE") == 0)
		setDefenseupgrades(getDefenseupgrades() + 1);
	else if (type.compare(L"SPEED") == 0)
		setSpeedupgrades(getSpeedupgrades() + 1);

	int time = rand() % 10 + 10;
	time = time * 40;
	u->setTime(time);

	upgrades.push_back(u); 
}

void GameStateManager::useUpgrade(wstring type)
{
	Game *game = Game::getSingleton();
	PlayerSprite *player = spriteManager->getPlayer();

	TextGenerator *generator = game->getText()->getTextGenerator();

	list<Upgrade*>::iterator upgradeIterator = getUpgradesIterator();
	list<Upgrade*>::iterator end = getEndOfUpgradesIterator();
	while (upgradeIterator != end && !getUpgrades().empty())
	{
		Upgrade *upgrade = (*upgradeIterator);

		//if (upgrade->getWasActivated() == false)
		{
			//if (upgrade->getActivated() == true)
			{
				
				if (upgrade->getType().compare(type) == 0)
				{
					if (type.compare(L"ATTACK") == 0)
					{
						if (attackupgrades > 0)
						{
							player->setAttack(player->getAttack() + 1);
							setAttackupgrades(getAttackupgrades() - 1);
						}
					}
					else if (type.compare(L"DEFENSE") == 0)
					{
						if (defenseupgrades > 0)
						{
							player->setDefense(player->getDefense() + 1);
							setDefenseupgrades(getDefenseupgrades() - 1);
						}
					}
					else if (type.compare(L"SPEED") == 0)
					{
						if (speedupgrades > 0)
						{
							player->setSpeed(player->getSpeed() + 1);
							setSpeedupgrades(getSpeedupgrades() - 1);
						}
					}

					upgrade->setWasActivated(true);
					upgrade->setActivated(true);
					break;
				}
			}
		}

		upgradeIterator++;
	}
}

wstring GameStateManager::getKey()
{
	switch (getSafety())
	{
	case(0) :
		return L"Q";
		break;
	case(1) :
		return L"W";
		break;
	case(2) :
		return L"E";
		break;
	case(3) :
		return L"A";
		break;
	case(4) :
		return L"S";
		break;
	case(5) :
		return L"D";
		break;
	case(6) :
		return L"Z";
		break;
	case(7) :
		return L"X";
		break;
	case(8) :
		return L"C";
		break;
	}
}

int GameStateManager::getIntKey()
{
	switch (getSafety())
	{
	case(0) :
		return (unsigned int)'Q';
		break;
	case(1) :
		return (unsigned int)'W';
		break;
	case(2) :
		return (unsigned int)'E';
		break;
	case(3) :
		return (unsigned int)'A';
		break;
	case(4) :
		return (unsigned int)'S';
		break;
	case(5) :
		return (unsigned int)'D';
		break;
	case(6) :
		return (unsigned int)'Z';
		break;
	case(7) :
		return (unsigned int)'X';
		break;
	case(8) :
		return (unsigned int)'C';
		break;
	}
}

