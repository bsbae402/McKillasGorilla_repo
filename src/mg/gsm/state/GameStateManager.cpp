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
}

void GameStateManager::goToPreGame()
{
	currentGameState = GS_PRE_GAME;
}

void GameStateManager::goToIngamePauseMenu()
{
	currentGameState = GS_PAUSED;
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

/*
	isWorldRenderable - Used to test if the game world should be rendered
	or not. Note that even if the game is paused, you'll likely still render
	the game.
*/
bool GameStateManager::isWorldRenderable()
{
	return (	(currentGameState == GS_GAME_IN_PROGRESS)
		||		(currentGameState == GS_PAUSED)
		||		(currentGameState == GS_GAME_OVER));
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
		loadLevel(currentLevelIndex);
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

	if (physics.isActivated() || physics.isActivatedForSingleUpdate())
	{
		physics.update();
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

