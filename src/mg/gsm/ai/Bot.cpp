#include "mg_VS\stdafx.h"
#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"

Bot::~Bot()
{
	// DELETE ALL THE BEHAVIORS, SINCE THEY ARE UNIQUE TO EACH Bot
	map<BotState, BotBehavior*>::iterator behaviorIt = behaviors.begin();
	while (behaviorIt != behaviors.end())
	{
		map<BotState, BotBehavior*>::iterator tempIt = behaviorIt;
		behaviorIt++;
		BotBehavior *behaviorToDelete = tempIt->second;
		delete behaviorToDelete;
	}
	behaviors.clear();
}

Bot* Bot::clone()
{
	Bot *clonedBot = new Bot();
	clonedBot->alpha = alpha;
	clonedBot->animationCounter = animationCounter;
	clonedBot->behaviors[MOVING] = behaviors[BotState::MOVING]->clone();
	clonedBot->behaviors[DYING] = behaviors[BotState::DYING]->clone();
	clonedBot->behaviors[SPAWNING] = behaviors[BotState::SPAWNING]->clone();
	clonedBot->botState = botState;
	clonedBot->currentlyCollidable = currentlyCollidable;
	clonedBot->currentState = currentState;
	clonedBot->frameIndex = frameIndex;
	clonedBot->rotationInRadians = rotationInRadians;
	clonedBot->spriteType = spriteType;
	clonedBot->type = type;
	return clonedBot;
}

void Bot::think()
{
	if (botState == BotState::MOVING)
	{
		behaviors[BotState::MOVING]->behave(this);
	}
	else if (botState == BotState::DYING)
	{
		behaviors[BotState::DYING]->behave(this);
	}
}
