#pragma once

#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"

class RandomIntervalMovement : public BotBehavior
{
private:
	long long int timeIntervalMax;
	long long int intervalRemaining;
	long long int lastTime;

public:
	RandomIntervalMovement() {}
	~RandomIntervalMovement() {}
	void setIntervalMax(float initTimeIntervalMax)
	{
		timeIntervalMax = initTimeIntervalMax;
	}

	// DEFINED IN RandomIntervalMovement.cpp
	void behave(Bot *bot);
	BotBehavior* clone();
};