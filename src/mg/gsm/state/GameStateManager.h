/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameStateManager.h

	This class manages the static data for the game level being
	used. This means all game backgrounds, which are stored and
	manipulated in WorldLayer objects. A given level can have
	a game background rendered using multiple layers, some which
	can be tiled, others that can be sparse, and others that
	can be isometric.

	This class stores these layers and ensures they are rendered
	in the proper order, low index to high. Layers that need to
	be drawn first (the back-most layer), should be added first.
*/
#pragma once
#include "mg_VS\stdafx.h"
#include "mg\graphics\RenderList.h"
#include "mg\gsm\state\GameState.h"
#include "mg\gsm\physics\Physics.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateMachine.h"
#include "mg\gsm\world\World.h"
#include "mg\gsm\world\WorldLayer.h"
#include "mg\gsm\ai\pathfinding\OrthographicGridPathfinder.h"

#include "mg\gsm\sprite\Upgrade.h"


class Game;

static const int NO_LEVEL_LOADED = -1;

class GameStateManager
{
private:
	bool lose;

	// THE CURRENT GAME STATE
	GameState currentGameState;

	// THE CURRENT LEVEL IS THE ONE CURRENTLY BEING PLAYED. NOTE THAT IF
	// THE CURRENT LEVEL IS 0, THEN IT MEANS THERE IS NO ACTIVE LEVEL LOADED,
	// LIKE WHEN WE'RE AT THE MAIN MENU
	unsigned int currentLevelIndex;

	// THESE VECTORS STORE THE NAMES OF EACH LEVEL AND THE RELATIVE
	// PATH AND FILE NAME OF EACH DATA INPUT FILE FOR ALL GAME LEVELS. 
	// NOTE THAT WE'LL LOAD THESE VECTORS WITH THIS DATA WHEN THE GAME STARTS UP, 
	// BUT WE'LL ONLY LOAD THE LEVELS INTO THE GAME WORLD AS EACH LEVEL IS PLAYED
	// NOTE THAT THE LEVEL NAME AT INDEX i IN THE FIRST VECTOR CORRESPONDS
	// TO THE PATH AND FILE NAME AT INDEX i IN THE SECOND ONE
	vector<wstring> levelNames;
	vector<wstring> levelDirs;
	vector<wstring> levelFiles;

	// FOR MANAGING STATIC GAME WORLD DATA, i.e. BACKGROUND IMAGES,
	// TILES, PLATFORMS, etc. BASICALLY THINGS THAT ARE NOT ANIMATED
	// AND DO NOT MOVE
	World world;

	// FOR MANAGING DYNAMIC GAME OBJECTS FOR CURRENT LEVEL
	// NOTE THAT WE CALL THE DYNAMIC OBJECTS "SPRITES"
	SpriteManager *spriteManager;

	// THIS IS AI SYSTEM THAT MANAGES GAME STATE TRANSITIONS
	GameStateMachine *gameStateMachine;

	// FOR DOING ALL COLLISION DETECTION AND RESOLUTION
	Physics			physics;

	//PATHFINDING
	OrthographicGridPathfinder *path;

	list<Upgrade*> upgrades;


	int score = 0;
	int money = 0;
	int donatemoney = 0;
	int safety;
	int countdownCounter = 500;
	bool safetyon;
	bool moveviewport = false;

	int endlevelx;
	int endlevelxwidth;
	int endlevely;
	int endlevelywidth;

	int attackupgrades;
	int defenseupgrades;
	int speedupgrades;


public:
	void setLose(bool newlose) { lose = newlose; }
	bool getLose() { return lose; }

	// INLINED ACCESSOR METHODS
	GameState			getCurrentGameState()	{ return currentGameState;				}
	unsigned int		getCurrentLevelIndex()	{ return currentLevelIndex;				}
	unsigned int		getNumLevels()			{ return levelNames.size();				}
	Physics*			getPhysics()			{ return &physics;						}
	SpriteManager*		getSpriteManager()		{ return spriteManager;					}
	World*				getWorld()				{ return &world;						}
	GameStateMachine*	getGameStateMachine()	{ return gameStateMachine; }
	wstring				getCurrentLevelName()	{ return levelNames[currentLevelIndex];	}
	OrthographicGridPathfinder* getPath() { return path; }
	void setPath(OrthographicGridPathfinder *initPath) { path = initPath; }


	int getScore() { return score; }
	int getMoney() { return money; }
	int getDonateMoney() { return donatemoney; }
	int getSafety() { return safety; }
	void setScore(int newscore) { score = newscore; }
	void setMoney(int newmoney) { money = newmoney; }
	void setDonateMoney(int newdonatemoney) { donatemoney = newdonatemoney; }
	void setSafety(int newsafety) { safety = newsafety; }
	wstring getKey();
	int getIntKey();
	bool isSafetyon() { return safetyon; }
	void SafetyOn() { safetyon = true; }
	void SafetyOff() { safetyon = false; }
	//void setMoveviewport(bool newmoveviewport) { moveviewport = newmoveviewport; }
	//bool getMoveviewport() { return moveviewport; }

	void setEndlevelx(int newendlevelx) { endlevelx = newendlevelx; }
	void setEndlevelxwidth(int newendlevelxwidth) { endlevelxwidth = newendlevelxwidth; }
	void setEndlevely(int newendlevely) { endlevely = newendlevely; }
	void setEndlevelywidth(int newendlevelywidth) { endlevelywidth = newendlevelywidth; }

	int getEndlevelx() { return endlevelx; }
	int getEndlevelxwidth() { return endlevelxwidth; }
	int getEndlevely() { return endlevely; }
	int getEndlevelywidth() { return endlevelywidth; }

	bool getMoveviewport() { return moveviewport; }
	void setMoveviewport(bool newmove) { moveviewport = newmove; }
	void toggleMoveviewport() { moveviewport = !moveviewport; }

	// INLINED MUTATOR METHOD
	void setGameStateMachine(GameStateMachine *initBotStateManager)
	{
		gameStateMachine = initBotStateManager;
	}

	// METHODS FOR TESTING THE CURRENT GAME STATE
	bool			isAppActive();
	bool			isAtSplashScreen();
	bool			isGameInProgress();
	bool			isGameLevelLoading();
	bool			isPreGame();
	bool			isWorldRenderable();
	////// -- true if game is in pause menu
	bool			isGameInPauseMenu();
	bool			isGameAtUpgradeScreen();
	bool			isGameAtDonateScreen();

	// METHODS FOR TRANSITIONING TO OTHER GAME STATES
	void			goToGame();
	void			goToLoadLevel();
	void			goToMainMenu();
	void			goToControlsMenu();
	void			goToPreGame();
	//// -- go to ingame menu
	void			goToIngamePauseMenu();
	//// -- go to upgrade screen
	void			goToUpgradeScreen();
	void			goToDonateScreen();
	//// -- go to level complete (when player reached the end of the level)
	void			goToLevelComplete();
	//// -- go to credits screen
	void			goToCreditsScreen();
	//// -- go to game over state
	void			goToGameOverScreen();

	// METHODS DEFINED in GameStateManager.cpp
	GameStateManager();
	~GameStateManager();
	void			addGameRenderItemsToRenderList();
	void			addLevel(wstring levelToAddName, wstring levelToAddDir, wstring levelToAddFile);
	unsigned int	getLevelNum(wstring levelName);
	void			loadLevel(unsigned int levelNum);
	void			loadLevel(wstring levelName);
	void			loadCurrentLevel();
	void			loadNextLevel();
	void			unloadCurrentLevel();
	void			update();
	void			startUp();
	void			shutDown();

	list<Upgrade*> getUpgrades() { return upgrades; }
	void addUpgrade(wstring type);
	void removeUpgrade(Upgrade *u) { upgrades.remove(u); }
	void useUpgrade(wstring type);

	void setAttackupgrades(int attack) { attackupgrades = attack; }
	void setDefenseupgrades(int defense) { defenseupgrades = defense; }
	void setSpeedupgrades(int speed) { speedupgrades = speed; }

	int getAttackupgrades() { return attackupgrades; }
	int getDefenseupgrades() { return defenseupgrades; }
	int getSpeedupgrades() { return speedupgrades; }
	list<Upgrade*>::iterator	getUpgradesIterator() { return upgrades.begin(); }
	list<Upgrade*>::iterator	getEndOfUpgradesIterator() { return upgrades.end(); }

};