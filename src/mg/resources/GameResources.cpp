#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\world\World.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGEngineConfigImporter.h"
#include "mg\resources\importers\MGLevelImporter.h"
#include "mg\resources\importers\PoseurSpriteTypeImporter.h"
#include "mg\resources\importers\TMXMapImporter.h"
#include "tinyxml\tinystr.h";
#include "tinyxml\tinyxml.h";

void GameResources::startUp()
{
	engineConfigImporter = new MGEngineConfigImporter();
	levelImporter = new MGLevelImporter();
	mapImporter = new TMXMapImporter();
	spriteTypeImporter = new PoseurSpriteTypeImporter();
}

void GameResources::shutDown()
{
	delete engineConfigImporter;
	delete spriteTypeImporter;
	delete mapImporter;
	delete levelImporter;
}