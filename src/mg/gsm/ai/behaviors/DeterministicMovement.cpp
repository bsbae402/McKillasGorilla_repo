#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\DeterministicMovement.h"
#include "mg\gsm\physics\PhysicalProperties.h"

void DeterministicMovement::behave(Bot *bot)
{

}

BotBehavior* DeterministicMovement::clone()
{
	DeterministicMovement *behavior = new DeterministicMovement();
	return behavior;
}

