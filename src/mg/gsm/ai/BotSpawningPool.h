#pragma once
#include "mg_VS/stdafx.h"

enum BotSpawningPoolType
{
	RANDOM,
	SCHEDULED
};

static const string RANDOM_SPAWNING_POOL_TYPE = "random";
static const string SCHEDULED_SPAWNING_POOL_TYPE = "scheduled";

class BotSpawningPool
{
private:
	BotSpawningPoolType spawningPoolType;
	wstring botType;
	int x;
	int y;
	int maxInterval;
	int countdownCounter;

public:
	BotSpawningPoolType getSpawningPoolType() { return spawningPoolType; }
	wstring getWstringForType(BotSpawningPoolType type)
	{
		if (type == RANDOM) return wstring(RANDOM_SPAWNING_POOL_TYPE.begin(), RANDOM_SPAWNING_POOL_TYPE.end());
		else if (type == SCHEDULED) return wstring(SCHEDULED_SPAWNING_POOL_TYPE.begin(), SCHEDULED_SPAWNING_POOL_TYPE.end());
		else return L"";
	}
	BotSpawningPoolType getBotSpawningPoolTypeForString(string typeAsText) {
		if (typeAsText.compare(RANDOM_SPAWNING_POOL_TYPE) == 0)
			return BotSpawningPoolType::RANDOM;
		else
			return BotSpawningPoolType::SCHEDULED;
	}
	int getX() { return x; }
	int getY() { return y; }

	// THESE ARE IN BotSpawningPool.cpp
	BotSpawningPool(string initSpawningPoolType, wstring initBotType, int initX, int initY, int initMaxInterval);
	void initCountdownCounter();
	void update();
};