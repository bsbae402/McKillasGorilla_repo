#pragma once

#include "mg\gsm\ai\behaviors\BotBehavior.h"

class RecycledDeath : public BotBehavior
{
public:
	RecycledDeath() {}
	~RecycledDeath() {}
	void behave(Bot *bot);
	BotBehavior* clone();
};