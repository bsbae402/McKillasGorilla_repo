/**
	This class provides premade lists of commonly used bots to avoid
	dynamic construction during gameplay. 

**/
#pragma once
#include "mg_VS\stdafx.h"
#include "mg\gsm\ai\Bot.h"

static const unsigned int RECYCLABLE_BOT_INCREMENT = 10;

class BotRecycler
{
private:
	map<wstring, list<Bot *> *> recyclableBots;
	map<wstring, Bot *> registeredBotTypes;

public:
	BotRecycler();
	~BotRecycler();
	map<wstring, list<Bot *> *>* getRecyclableBots() { return &recyclableBots; }
	map<wstring, Bot *>* getRegisteredBotTypes() { return &registeredBotTypes; }
	void addMoreBots(wstring botType, unsigned int numToAdd);
	bool isRegisteredBotType(wstring botType);
	void recycleBot(Bot* botToRecycle);
	void registerBotType(wstring botType, Bot *firstBot);
	Bot* retrieveBot(wstring botType);
	void unload();
};