#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\behaviors\BotBehavior.h"
#include "mg\gsm\ai\behaviors\RandomSpawn.h"
#include "mg\gsm\physics\PhysicalProperties.h"

void RandomSpawn::behave(Bot *bot)
{
	// PICK A RANDOM VELOCITY
	std::random_device rd;
	std::uniform_real_distribution<float> dist(0, maxVelocity);
	float totalVelocity = dist(rd);

	// AND A RANDOM DIRECTION
	std::uniform_int_distribution<int> degreesDist(0, 360);
	int degrees = degreesDist(rd);
	float radians = degrees * PI / 180;
	float vX = 0.0f;
	float vY = 0.0f;
	if ((degrees == 90) || (degrees == 270)) {
		vY = 0.0f;
	}
	else if ((degrees == 0) || (degrees == 180) || (degrees == 360)) {
		vX = 0.0f;
	}
	else {
		vX = totalVelocity * sin(radians);
		vY = totalVelocity * cos(radians);
	}
	PhysicalProperties *pp = bot->getPhysicalProperties();
	pp->setVelocity(vX, vY);

	// AND THEN CHANGE THE BOT STATE
	bot->setBotState(BotState::MOVING);

	// AND THE ANIMATION STATE
	bot->setCurrentState(L"WALKING");
}

BotBehavior* RandomSpawn::clone()
{
	RandomSpawn *behavior = new RandomSpawn();
	behavior->maxVelocity = maxVelocity;
	return behavior;
}

