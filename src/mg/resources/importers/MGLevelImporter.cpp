#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\DeterministicMovement.h"
#include "mg\gsm\ai\behaviors\RandomIntervalMovement.h"
#include "mg\gsm\ai\behaviors\RandomSpawn.h"
#include "mg\gsm\ai\behaviors\RecycledDeath.h"
#include "mg\gsm\ai\behaviors\RespawnedDeath.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\ai\BotSpawningPool.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGLevelImporter.h"
#include "mg\resources\importers\PoseurSpriteTypeImporter.h"
#include "mg\resources\importers\TMXMapImporter.h"
#include "mg\text\GameText.h"
#include "tinyxml\tinystr.h";
#include "tinyxml\tinyxml.h";

#include "mg\gsm\sprite\LevelObjectSprite.h"

bool MGLevelImporter::load(wstring levelFileDir, wstring levelFile)
{
	// FOR PRINTING DEBUGGING TEXT
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameText *text = game->getText();

	// LOAD THE XML DOC
	const char *charPath = xmlReader.newCharArrayFromWstring(levelFileDir + levelFile);
	TiXmlDocument doc(charPath);
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		TiXmlElement *level = doc.FirstChildElement();
		
		// WHAT'S THE NAME OF THE LEVEL?
		const char* levelName = level->Attribute(MG_LEVEL_NAME_ATT.c_str());
		string textToWrite(levelName);
		wstring wTextToWrite(textToWrite.begin(), textToWrite.end());
		text->writeDebugOutput(L"Level Name: " + wTextToWrite);
		
		// THE LEVEL NODE HAS 4 CHILD NODES TO LOAD

		// level_map
		TiXmlElement *levelMap = level->FirstChildElement();

		// level_map DIR
		const char* mapDir = levelMap->Attribute(MG_MAP_DIR_ATT.c_str());
		string dir(mapDir);
		wstring wDir(dir.begin(), dir.end());
		text->writeDebugOutput(L"Map Dir: " + wDir);

		// level_map FILE
		const char* mapFile = levelMap->Attribute(MG_MAP_FILE_ATT.c_str());
		string file(mapFile);
		wstring wFile(file.begin(), file.end());
		text->writeDebugOutput(L"Map File: " + wFile);

		// NOW LOAD THE MAP
		GameResources *resources = game->getResources();
		TMXMapImporter *mapImporter = (TMXMapImporter*)resources->getMapImporter();
		mapImporter->load(wDir, wFile);

		// level_sprite_types
		TiXmlElement *levelSpriteTypes = levelMap->NextSiblingElement();

		// sprite_types DIR
		const char* spriteTypesDir = levelSpriteTypes->Attribute(MG_SPRITE_TYPES_DIR_ATT.c_str());
		dir = string(spriteTypesDir);
		wDir = wstring(dir.begin(), dir.end());
		text->writeDebugOutput(L"Sprite Types Dir: " + wDir);

		// sprite_types FILE
		const char* spriteTypesFile = levelSpriteTypes->Attribute(MG_SPRITE_TYPES_FILE_ATT.c_str());
		file = string(spriteTypesFile);
		wFile = wstring(file.begin(), file.end());
		text->writeDebugOutput(L"Sprite Types File: " + wFile);

		// AND LOAD THE SPRITE TYPES
		PoseurSpriteTypeImporter *spriteTypesImporter = (PoseurSpriteTypeImporter*)resources->getSpriteTypeImporter();
		spriteTypesImporter->load(wDir, wFile);

		// NOW THAT WE KNOW THE SPRITE TYPES LET'S SETUP
		// THE BotRecycler SO THAT WE CAN EASILY
		// MAKE AnimatedSprites AND SPAWN WITHOUT PENALTY
		SpriteManager *spriteManager = gsm->getSpriteManager();
		BotRecycler *botRecycler = spriteManager->getBotRecycler();

		// level_bot_types
		TiXmlElement *botTypesList = levelSpriteTypes->NextSiblingElement();
		TiXmlElement *botType = botTypesList->FirstChildElement();
		while (botType != nullptr)
		{
			// FIRST GET THE BOT TYPE
			const char* botTypeName = xmlReader.extractCharAtt(botType, MG_BOT_TYPE_ATT);
			string strBotTypeName(botTypeName);
			wstring wBotTypeName(strBotTypeName.begin(), strBotTypeName.end());
			text->writeDebugOutput(L"Bot Type: " + wBotTypeName);

			// THEN GET THE SPRITE TYPE
			const char* spriteType = botType->Attribute(MG_SPRITE_TYPE_ATT.c_str());
			string strSpriteType(spriteType);
			wstring wSpriteType(strSpriteType.begin(), strSpriteType.end());
			text->writeDebugOutput(L"Bot's Sprite Type: " + wSpriteType);

			const char* movingBehavior = botType->Attribute(MG_MOVING_BEHAVIOR.c_str());
			string strMovingBehavior(movingBehavior);
			wstring wStrMovingBehavior(strMovingBehavior.begin(), strMovingBehavior.end());
			text->writeDebugOutput(L"bot moving behavior: " + wStrMovingBehavior);

			const char* dyingBehavior = botType->Attribute(MG_DYING_BEHAVIOR.c_str());
			string strDyingBehavior(dyingBehavior);
			wstring wStrDyingBehavior(strDyingBehavior.begin(), strDyingBehavior.end());
			text->writeDebugOutput(L"bot dying behavior: " + wStrDyingBehavior);

			const char* spawningBehavior = botType->Attribute(MG_SPAWNING_BEHAVIOR.c_str());
			string strSpawningBehavior(spawningBehavior);
			wstring wStrSpawningBehavior(strSpawningBehavior.begin(), strSpawningBehavior.end());
			text->writeDebugOutput(L"bot spawning behavior: " + wStrSpawningBehavior);

			// MAKE THE FIRST BOT FOR THIS TYPE
			Bot *firstBot = new Bot();
			AnimatedSpriteType *botSpriteType = spriteManager->getSpriteType(wSpriteType);
			firstBot->setSpriteType(botSpriteType);
			firstBot->setAlpha(255);
			firstBot->setBotState(DEAD);
			firstBot->setType(wBotTypeName);
			
			// WHICH MOVEMENT BEHAVIOR?
			if (strMovingBehavior.compare(MG_MOVING_DETERMINISTIC_BEHAVIOR) == 0)
			{
				firstBot->setBehavior(firstBot->getBotStateForString(MG_MOVING_BEHAVIOR), new DeterministicMovement());
			}
			else if (strMovingBehavior.compare(MG_MOVING_RANDOM_INTERVAL_BEHAVIOR) == 0)
			{
				firstBot->setBehavior(firstBot->getBotStateForString(MG_MOVING_BEHAVIOR), new RandomIntervalMovement());
			}

			// AND WHICH DEATH BEHAVIOR?
			if (strDyingBehavior.compare(MG_DYING_RECYCLE_BEHAVIOR) == 0)
			{
				firstBot->setBehavior(firstBot->getBotStateForString(MG_DYING_BEHAVIOR), new RecycledDeath());
			}
			else if (strDyingBehavior.compare(MG_DYING_RESPAWN_BEHAVIOR) == 0)
			{
				firstBot->setBehavior(firstBot->getBotStateForString(MG_DYING_BEHAVIOR), new RespawnedDeath());
			}

			// AND WHICH SPAWN BEHAVIOR?
			if (strSpawningBehavior.compare(MG_SPAWNING_RANDOM_BEHAVIOR) == 0)
			{
				int maxVelocity = xmlReader.extractIntAtt(botType, MG_MAX_VELOCITY);
				RandomSpawn *randomSpawn = new RandomSpawn();
				randomSpawn->setVelocity(maxVelocity);
				firstBot->setBehavior(firstBot->getBotStateForString(MG_SPAWNING_BEHAVIOR), randomSpawn);
			}
			// TELL THE BOT RECYCLER ABOUT THIS BOT TYPE
			botRecycler->registerBotType(wBotTypeName, firstBot);

			// ON TO THE NEXT BOT TYPE
			botType = botType->NextSiblingElement();
		}

		// NOW FOR INITIAL SPRITES
		TiXmlElement *botsList = botTypesList->NextSiblingElement();
		TiXmlElement *bot = botsList->FirstChildElement();
		while (bot != nullptr)
		{
			int initX = xmlReader.extractIntAtt(bot, MG_INIT_X_ATT);
			int initY = xmlReader.extractIntAtt(bot, MG_INIT_Y_ATT);
			int initVx = xmlReader.extractIntAtt(bot, MG_INIT_VX_ATT);
			int initVy = xmlReader.extractIntAtt(bot, MG_INIT_VY_ATT);
			wstring debugText = L"Bot x, y, vX, vY: ";
			wstringstream wss;
			wss << initX; wss << L", "; wss << initY; wss << initVx; wss << L","; wss << initVy;
			debugText += wss.str();
			text->writeDebugOutput(debugText);

			const char* initialBotState = xmlReader.extractCharAtt(bot, MG_INIT_BOT_STATE_ATT);
			string strInitialBotState(initialBotState);
			wstring wInitialBotState(strInitialBotState.begin(), strInitialBotState.end());
			text->writeDebugOutput(L"init bot state: " + wInitialBotState);

			const char* initSpriteState = xmlReader.extractCharAtt(bot, MG_INIT_SPRITE_STATE_ATT);
			string strInitialSpriteState(initSpriteState);
			wstring wInitialSpriteState(strInitialSpriteState.begin(), strInitialSpriteState.end());
			text->writeDebugOutput(L"init sprite state: " + wInitialSpriteState);

			// NOW GET THE BOT TYPE
			const char* botTypeName = xmlReader.extractCharAtt(bot, MG_BOT_TYPE_ATT);
			string strBotTypeName(botTypeName);
			wstring wBotTypeName(strBotTypeName.begin(), strBotTypeName.end());
			text->writeDebugOutput(L"Bot Type: " + wBotTypeName);

			// GET A BOT
			Bot* botToSpawn = botRecycler->retrieveBot(wBotTypeName);

			// INIT THE BOT WITH THE DATA WE'VE PULLED OUT
			PhysicalProperties *pp = botToSpawn->getPhysicalProperties();
			pp->setPosition(initX, initY);
			pp->setVelocity(initVx, initVy);
			BotState botState = botToSpawn->getBotStateForString(initialBotState);
			botToSpawn->setBotState(botState);
			botToSpawn->setCurrentState(wInitialSpriteState);
			botToSpawn->setRotationInRadians(PI / 2);

			// AND GIVE IT TO THE SPRITE MANAGER
			spriteManager->addBot(botToSpawn);

			// ONTO THE NEXT BOT
			bot = bot->NextSiblingElement();
		}

		// spawning_pools
		TiXmlElement *spawningPoolsList = botsList->NextSiblingElement();
		TiXmlElement *spawningPool = spawningPoolsList->FirstChildElement();
		while (spawningPool != nullptr)
		{
			int initX = xmlReader.extractIntAtt(spawningPool, MG_X_ATT);
			int initY = xmlReader.extractIntAtt(spawningPool, MG_Y_ATT);

			// GET THE INTERVAL TYPE
			const char* intervalTypeChar = xmlReader.extractCharAtt(spawningPool, MG_INTERVAL_TYPE_ATT);
			string strIntervalType(intervalTypeChar);

			// NOW GET THE BOT TYPE
			const char* botTypeName = xmlReader.extractCharAtt(spawningPool, MG_BOT_TYPE_ATT);
			string strBotTypeName(botTypeName);
			wstring wBotTypeName(strBotTypeName.begin(), strBotTypeName.end());
			text->writeDebugOutput(L"Bot Type: " + wBotTypeName);

			// AND THE MAX INTERVAL
			int maxInterval = xmlReader.extractIntAtt(spawningPool, MG_INTERVAL_TIME_ATT);
			BotSpawningPool *pool = new BotSpawningPool(strIntervalType, wBotTypeName, initX, initY, maxInterval);

			// GIVE THE SPAWNING POOL TO THE SPRITE MANAGER
			spriteManager->addSpawningPool(pool);

			// ONTO THE NEXT SPAWNING POOL
			spawningPool = spawningPool->NextSiblingElement();
		}


		//LOAD THE PLAYER
		TiXmlElement *playerList = spawningPoolsList->NextSiblingElement();
		TiXmlElement *player = playerList->FirstChildElement();

		while (player != nullptr) {

			// THEN GET THE SPRITE TYPE
			const char* spriteType = player->Attribute(MG_SPRITE_TYPE_ATT.c_str());
			string strSpriteType(spriteType);
			wstring wSpriteType(strSpriteType.begin(), strSpriteType.end());
			text->writeDebugOutput(L"Players's Sprite Type: " + wSpriteType);


			int initX = xmlReader.extractIntAtt(player, MG_INIT_X_ATT);
			int initY = xmlReader.extractIntAtt(player, MG_INIT_Y_ATT);
			wstring debugText = L"Bot x, y: ";
			wstringstream wss;
			wss << initX; wss << L", "; wss << initY;
			debugText += wss.str();
			text->writeDebugOutput(debugText);

			const char* initialBotState = xmlReader.extractCharAtt(player, MG_INIT_BOT_STATE_ATT);
			string strInitialBotState(initialBotState);
			wstring wInitialBotState(strInitialBotState.begin(), strInitialBotState.end());
			text->writeDebugOutput(L"init bot state: " + wInitialBotState);

			const char* initSpriteState = xmlReader.extractCharAtt(player, MG_INIT_SPRITE_STATE_ATT);
			string strInitialSpriteState(initSpriteState);
			wstring wInitialSpriteState(strInitialSpriteState.begin(), strInitialSpriteState.end());
			text->writeDebugOutput(L"init sprite state: " + wInitialSpriteState);


			// MAKE THE PLAYER
			Bot *playerBot = new Bot();
			AnimatedSpriteType *playerSpriteType = spriteManager->getSpriteType(wSpriteType);
			playerBot->setSpriteType(playerSpriteType);
			playerBot->setAlpha(255);

			PhysicalProperties *pp = playerBot->getPhysicalProperties();
			pp->setPosition(initX, initY);
			pp->setVelocity(0, 0);
			BotState botState = playerBot->getBotStateForString(initialBotState);
			playerBot->setBotState(botState);
			playerBot->setCurrentState(wInitialSpriteState);
			playerBot->setRotationInRadians(PI / 2);


			spriteManager->setPlayer(playerBot);

			player = player->NextSiblingElement();
		}

		//// ---- loading level objects ---- ////
		TiXmlElement *levelObjectsList = playerList->NextSiblingElement();
		TiXmlElement *levelObject = levelObjectsList->FirstChildElement();
		while (levelObject != nullptr) {
			//// 1. info collecting sequence
			//// - levelObjectType
			const char* levelObjectType = levelObject->Attribute(MG_LEVEL_OBJECT_TYPE_ATT.c_str());
			string strLevelObjectType(levelObjectType);
			wstring wStrLevelObjectType(strLevelObjectType.begin(), strLevelObjectType.end());

			//// - spriteType
			const char* spriteType = levelObject->Attribute(MG_SPRITE_TYPE_ATT.c_str());
			string strSpriteType(spriteType);
			wstring wSpriteType(strSpriteType.begin(), strSpriteType.end());

			//// - levelObjectID
			int levelObjectID = xmlReader.extractIntAtt(levelObject, MG_LEVEL_OBJECT_ID_ATT);

			//// - object location
			int initX = xmlReader.extractIntAtt(levelObject, MG_INIT_X_ATT);
			int initY = xmlReader.extractIntAtt(levelObject, MG_INIT_Y_ATT);

			//// 2. make object
			//// - set sprite type
			LevelObjectSprite *los = new LevelObjectSprite();
			AnimatedSpriteType *losType = spriteManager->getSpriteType(wSpriteType);
			los->setSpriteType(losType);
			los->setAlpha(255);

			//// - set physical property -> actually level objects normally don't have any velocity
			PhysicalProperties *pp = los->getPhysicalProperties();
			pp->setPosition(initX, initY);
			pp->setVelocity(0, 0);

			//// the initial sprite (animation) state for the level object is only "BEING"
			wstring wStrBeingState(BEING_LEVEL_OBJECT_SPRITE_STATE_VALUE.begin(), BEING_LEVEL_OBJECT_SPRITE_STATE_VALUE.end());
			los->setCurrentState(wStrBeingState);

			spriteManager->addLevelObject(los);

			levelObject = levelObject->NextSiblingElement();
		}
		//// ----	end	---- ////
	}
	return true;
}


bool MGLevelImporter::unload()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	gsm->unloadCurrentLevel();
	GameGraphics *graphics = game->getGraphics();
	graphics->clearWorldTextures();
	return true;
}