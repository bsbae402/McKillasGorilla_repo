#pragma once

#include "mg\gsm\ai\behaviors\BotBehavior.h"

class RespawnedDeath : public BotBehavior
{
private:
	bool alreadyRespawned;

public:
	RespawnedDeath() { alreadyRespawned = false; }
	~RespawnedDeath() {}
	void behave(Bot *bot);
	BotBehavior* clone();
};