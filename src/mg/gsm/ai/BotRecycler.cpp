#include "mg_VS\stdafx.h"
#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\sprite\SpriteManager.h"

BotRecycler::BotRecycler()
{
	srand((unsigned int)time(NULL));
}

/*
	A BotRecycler is constructed only once while a game app is
	running, at startup. When it is deconstructed, it must delete
	all remaining bots.
*/
BotRecycler::~BotRecycler()
{
	// GO THROUGH ALL THE TYPES OF BOTS AND DELETE ALL OF THEM
}

/*
	It is assumed that a game will have many different types of Bots, thinking
	in different ways, some of them even custom bots defined by the game app rather
	than by this framework. So, in order to make these bots recyclable, we have to
	register them with this recycler so that this recycler knows how to make new
	ones as needed.
*/
void BotRecycler::registerBotType(wstring botType, Bot *firstBot)
{
	// FIRST MAKE SURE WE DON'T ALREADY HAVE A BUNCH OF RECYCLABLE
	// BOTS FOR THIS TYPE. TO DO THIS, WE USE SOME C++ WEIRDNESS.
	if (!isRegisteredBotType(botType))
	{
		// REGISTER THE BOT
		registeredBotTypes[botType] = firstBot;
		recyclableBots[botType] = new list<Bot*>();
		addMoreBots(botType, RECYCLABLE_BOT_INCREMENT - 1);
	}
}

bool BotRecycler::isRegisteredBotType(wstring botType)
{
	// FIRST MAKE SURE THIS IS A REGISTERED BOT TYPE,
	// IF IT IS NOT, WE NEED TO RETURN NULL
	map<wstring, Bot *>::iterator testIt = registeredBotTypes.find(botType);
	if (testIt == registeredBotTypes.end())
		return false;
	else
		return true;
}

Bot* BotRecycler::retrieveBot(wstring botType)
{
	// FIRST MAKE SURE THIS IS A REGISTERED BOT TYPE,
	// IF IT IS NOT, WE NEED TO RETURN NULL
	if (!isRegisteredBotType(botType))
	{
		return nullptr;
	}
	else
	{
		// GET THE CORRECT LIST OF BOT TYPES
		list<Bot *> *botsOfTypeWeNeed = recyclableBots[botType];

		// MAKE SURE WE ARE NOT OUT OF THIS TYPE OF BOT
		if(botsOfTypeWeNeed->size() == 0)
			addMoreBots(botType, RECYCLABLE_BOT_INCREMENT);

		// NOW GET THE LAST ELEMENT
		Bot* botToReturn = botsOfTypeWeNeed->back();
		botsOfTypeWeNeed->pop_back();
		return botToReturn;
	}
}

void BotRecycler::recycleBot(Bot* botToRecycle)
{
	wstring botType = botToRecycle->getType();
	if (isRegisteredBotType(botType))
	{
		list<Bot *> *recycleBots = recyclableBots[botType];
		recycleBots->push_back(botToRecycle);
	}
}

void BotRecycler::addMoreBots(wstring botType, unsigned int numToAdd)
{
	// NOW MAKE THE BOTS
	Bot* sampleBot = registeredBotTypes[botType];

	for (unsigned int i = 0; i < numToAdd; i++)
	{
		Bot *botClone = sampleBot->clone();
		recyclableBots[botType]->push_back(botClone);
	}
}

void BotRecycler::unload()
{
	// DELETE ALL THE RECYCLABLE BOTS
	map<wstring, list<Bot *> *>::iterator botsListIt = recyclableBots.begin();
	while (botsListIt != recyclableBots.end())
	{
		map<wstring, list<Bot *> *>::iterator tempIt = botsListIt;
		botsListIt++;
		list<Bot*> *botsListToDelete = tempIt->second;
		list<Bot*>::iterator botIt = botsListToDelete->begin();
		while (botIt != botsListToDelete->end())
		{
			list<Bot*>::iterator tempBotIt = botIt;
			botIt++;
			Bot *botToDelete = (*tempBotIt);
			delete botToDelete;
		}
		botsListToDelete->clear();
		delete botsListToDelete;
	}
	recyclableBots.clear();

	// AND DELETE THE REGISTERED BOTS
	map<wstring, Bot *>::iterator registeredBotTypesIt = registeredBotTypes.begin();
	while (registeredBotTypesIt != registeredBotTypes.end())
	{
		map<wstring, Bot *>::iterator tempIt = registeredBotTypesIt;
		registeredBotTypesIt++;
		Bot *botToDelete = tempIt->second;
		delete botToDelete;
	}
	registeredBotTypes.clear();
}