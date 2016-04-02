#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\RecycledDeath.h"
#include "mg\game\Game.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateManager.h"

void RecycledDeath::behave(Bot *bot)
{
	// FIRST MAKE SURE THE ANIMATION STATE OF THE BOT
	// PLAYS THE DYING ANIMATION
	if (bot->getCurrentState().compare(L"DYING") != 0) {
		bot->setCurrentState(L"DYING");
		bot->getPhysicalProperties()->setVelocity(0.0f, 0.0f);
	}
	else {
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

BotBehavior* RecycledDeath::clone()
{
	RecycledDeath *behavior = new RecycledDeath();
	return behavior;
}