#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\ai\BotSpawningPool.h"
#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\sprite\SpriteManager.h"

BotSpawningPool::BotSpawningPool(string initSpawningPoolType, wstring initBotType, int initX, int initY, int initMaxInterval)
{
	spawningPoolType = getBotSpawningPoolTypeForString(initSpawningPoolType);
	x = initX;
	y = initY;
	botType = initBotType;
	maxInterval = initMaxInterval;
	initCountdownCounter();
}

void BotSpawningPool::initCountdownCounter()
{
	// NOW RESET THE COUNTDOWN COUNTER ACCORDING
	// TO THE TYPE OF SPAWNING POOL THIS IS
	if (spawningPoolType == BotSpawningPoolType::SCHEDULED)
		countdownCounter = maxInterval;
	else if (spawningPoolType == BotSpawningPoolType::RANDOM) {
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, maxInterval);
		countdownCounter = dist(rd);
	}
}

void BotSpawningPool::update()
{
	countdownCounter--;
	if (countdownCounter <= 0) {
		// SPAWN A BOT
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteManager = gsm->getSpriteManager();
		BotRecycler *botRecycler = spriteManager->getBotRecycler();
		Bot *spawnedBot = botRecycler->retrieveBot(botType);
		spriteManager->addBot(spawnedBot);
		initCountdownCounter();

		// DO IT'S SPAWNING BEHAVIOR
		BotBehavior *spawningBehavior = spawnedBot->getBehavior(BotState::SPAWNING);
		spawningBehavior->behave(spawnedBot);

		// AND START IT LOCATED AT THE SPAWNING POOL
		PhysicalProperties *pp = spawnedBot->getPhysicalProperties();
		pp->setX(x);
		pp->setY(y);
	}
}