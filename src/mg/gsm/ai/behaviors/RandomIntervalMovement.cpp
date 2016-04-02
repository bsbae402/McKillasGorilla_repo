#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\RandomIntervalMovement.h"
#include "mg\gsm\physics\PhysicalProperties.h"

void RandomIntervalMovement::behave(Bot *bot)
{
	// HOW MUCH TIME HAS PASSED SINCE THE LAST TIME WE CHECKED?

	// IS IT TIME TO CHANGE DIRECTION AGAIN?

}

BotBehavior* RandomIntervalMovement::clone()
{
	RandomIntervalMovement *behavior = new RandomIntervalMovement();
	return behavior;
}

