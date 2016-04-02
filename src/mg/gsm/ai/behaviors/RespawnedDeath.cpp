#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\ai\BotSpawningPool.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\RespawnedDeath.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateManager.h"

void RespawnedDeath::behave(Bot *bot)
{
	// RESPAWN THIS BOT AT A SPAWNING POOL
	if (!alreadyRespawned)
	{
		Game *game = Game::getSingleton();
		GameStateManager *gsm = game->getGSM();
		SpriteManager *spriteManager = gsm->getSpriteManager();
		vector<BotSpawningPool*> *spawningPools = spriteManager->getSpawningPools();

		// PICK A RANDOM SPAWNING POOL
		std::random_device rd;
		std::uniform_int_distribution<int> dist(0, spawningPools->size() - 1);
		int randomIndex = dist(rd);
		BotSpawningPool *spawningPool = spawningPools->at(randomIndex);

		// ADD THE BOT
		PhysicalProperties *pp = bot->getPhysicalProperties();
		pp->setX(spawningPool->getX());
		pp->setY(spawningPool->getY());

		// IT LIVES!!!
		bot->setBotState(BotState::MOVING);

		// MAKE SURE IT DOESN'T RESPAWN AGAIN
		alreadyRespawned = true;
	}
	else
	{
		// FIRST MAKE SURE THE ANIMATION STATE OF THE BOT
		// PLAYS THE DYING ANIMATION
		if (bot->getCurrentState().compare(L"DYING") != 0) {
			bot->setCurrentState(L"DYING");
			bot->getPhysicalProperties()->setVelocity(0.0f, 0.0f);
		}
		else
		{
			// CHECK TO SEE IF THE DYING ANIMATION IS DONE
			AnimatedSpriteType *spriteType = bot->getSpriteType();
			unsigned int frameIndex = (bot->getFrameIndex() * 2);
			unsigned int sequenceSize = spriteType->getSequenceSize(bot->getCurrentState()) + 2;
			if (frameIndex > sequenceSize) {
				// RETURN THIS BOT TO THE RECYCLE BIN
				Game *game = Game::getSingleton();
				GameStateManager *gsm = game->getGSM();
				SpriteManager *spriteManager = gsm->getSpriteManager();
				bot->markForRemoval();
				BotRecycler *botRecycler = spriteManager->getBotRecycler();
				botRecycler->recycleBot(bot);
			}
		}
	}
}

BotBehavior* RespawnedDeath::clone()
{
	RespawnedDeath *behavior = new RespawnedDeath();
	return behavior;
}