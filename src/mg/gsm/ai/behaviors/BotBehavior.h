#pragma once

#include "mg_VS\stdafx.h"

class Bot;

class BotBehavior
{
public:
	BotBehavior() {}
	virtual ~BotBehavior() {}
	virtual void behave(Bot *bot) = 0;
	virtual BotBehavior* clone() = 0;
};