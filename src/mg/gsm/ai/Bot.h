#pragma once

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\sprite\AnimatedSprite.h"

enum BotState
{
	DEAD,
	DYING,
	MOVING,
	SPAWNING,
	NONE
};

// BEHAVIORS
static const string MG_DEAD_BEHAVIOR = "DEAD";
static const string MG_DYING_BEHAVIOR = "DYING";
static const string MG_MOVING_BEHAVIOR = "MOVING";
static const string MG_SPAWNING_BEHAVIOR = "SPAWNING";
static const string MG_NONE = "none";
static const string MG_MOVING_DETERMINISTIC_BEHAVIOR = "deterministic";
static const string MG_MOVING_RANDOM_INTERVAL_BEHAVIOR = "random_interval";
static const string MG_DYING_RECYCLE_BEHAVIOR = "recycle";
static const string MG_DYING_RESPAWN_BEHAVIOR = "respawn";
static const string MG_SPAWNING_RANDOM_BEHAVIOR = "random";

class Bot : public AnimatedSprite
{
private:
	wstring type;
	BotState botState;
	map<BotState, BotBehavior*> behaviors;
	int health;
	int attack;
	int defense;
	int speed;
	int maxhealth;
	bool injured;
	bool startinjured;

public:
	Bot()	{}
	~Bot();

	void setHealth(int newhealth) { health = newhealth; }
	void setAttack(int newattack) { attack = newattack; }
	void setDefense(int newdefense) { defense = newdefense; }
	void setSpeed(int newspeed) { speed = newspeed; }
	void setMaxhealth(int newhealth) { maxhealth = newhealth; }
	void setInjured(bool newinjured) { injured = newinjured; }
	void setStartinjured(bool newinjured) { startinjured = newinjured; }

	int getHealth() { return health; }
	int getAttack() { return attack; }
	int getDefense() { return defense; }
	int getSpeed() { return speed; }
	int getMaxhealth() { return maxhealth; }
	bool getInjured() { return injured; }
	bool getStartinjured() { return startinjured; }

	// INLINED
	wstring getType() { return type; }
	BotState getBotState() { return botState; }
	BotBehavior* getBehavior(BotState state) { return behaviors[state]; }
	void setType(wstring initType)
	{
		type = initType;
	}
	void setBotState(BotState initState)
	{
		botState = initState;
	}
	void setBehavior(BotState state, BotBehavior *behavior)
	{
		behaviors[state] = behavior;
	}
	wstring getWstringForState(BotState state)
	{
		if (state == DEAD) return wstring(MG_DEAD_BEHAVIOR.begin(), MG_DEAD_BEHAVIOR.end());
		else if (state == DYING) return wstring(MG_DYING_BEHAVIOR.begin(), MG_DYING_BEHAVIOR.end());
		else if (state == MOVING) return wstring(MG_MOVING_BEHAVIOR.begin(), MG_MOVING_BEHAVIOR.end());
		else if (state == SPAWNING) return wstring(MG_SPAWNING_BEHAVIOR.begin(), MG_SPAWNING_BEHAVIOR.end());
		else return L"";
	}
	BotState getBotStateForString(string state)
	{
		if (state.compare(MG_DEAD_BEHAVIOR) == 0) return BotState::DEAD;
		else if (state.compare(MG_DYING_BEHAVIOR) == 0) return BotState::DYING;
		else if (state.compare(MG_MOVING_BEHAVIOR) == 0) return BotState::MOVING;
		else if (state.compare(MG_SPAWNING_BEHAVIOR) == 0) return BotState::SPAWNING;
		else return BotState::NONE;
	}

	// DEFINED IN Bot.cpp
	Bot* clone();
	void think();
};