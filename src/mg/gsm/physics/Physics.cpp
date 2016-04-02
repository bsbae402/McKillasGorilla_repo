/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	Physics.cpp

	See Physics.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\sprite\AnimatedSpriteType.h"
#include "mg\game\Game.h"
#include "mg\gsm\physics\Physics.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include <vector>
#include <list>

void Physics::startUp()
{
	activated = true;
}

void Physics::shutDown()
{

}

void Physics::update()
{
	if (activated || activatedForSingleUpdate)
	{
		Game *game = Game::getSingleton();
		SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
		GameStateManager *gsm = game->getGSM();
		World *world = gsm->getWorld();
		float worldWidth = world->getWorldWidth();
		float worldHeight = world->getWorldHeight();
		list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
		list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
		while (botIterator != end)
		{
			Bot *bot = (*botIterator);
			PhysicalProperties *pp = bot->getPhysicalProperties();
			pp->update();
			float left = pp->getX();
			float top = pp->getY();
			float right = pp->getX() + bot->getSpriteType()->getTextureWidth();
			float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();
			if ((left < 0) && (pp->getVelocityX() < 0))
				pp->setVelocity(-pp->getVelocityX(), pp->getVelocityY());
			if ((top < 0) && (pp->getVelocityY() < 0))
				pp->setVelocity(pp->getVelocityX(), -pp->getVelocityY());
			if ((right >= worldWidth) && (pp->getVelocityX() > 0))
				pp->setVelocity(-pp->getVelocityX(), pp->getVelocityY());
			if ((bottom >= worldHeight) && (pp->getVelocityY() > 0))
				pp->setVelocity(pp->getVelocityX(), -pp->getVelocityY());
			botIterator++;
		}
	}
	activatedForSingleUpdate = false;
}