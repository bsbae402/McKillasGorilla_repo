#pragma once

#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"

class DeterministicMovement : public BotBehavior
{
public:
	DeterministicMovement() {}
	~DeterministicMovement() {}
	void behave(Bot *bot);
	BotBehavior* clone();
};