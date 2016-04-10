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

#include "mg\input\GameInput.h"
#include "mg\gsm\sprite\PlayerSprite.h"

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

		//// --- player sprite physics
		GameInput *gameInput = game->getInput(); //// input may should not be handled in physics. consider one more.

		//// RebelleApp.h : MAX_VIEWPORT_AXIS_VELOCITY = 40.0f;
		PlayerSprite *playerSprite = spriteManager->getPlayer();
		PhysicalProperties *playerPP = playerSprite->getPhysicalProperties();
		
		//// set velocity of player
		PlayerState ps = playerSprite->getPlayerState();
		/// if player is not moving
		if (ps != ENUM_PLAYER_MOVING)
		{
			playerPP->setVelocity(0.0f, 0.0f);
		}

		/// ONLY if player is moving, then we set the velocity
		else 
		{
			PlayerDirection pd = playerSprite->getPlayerDirection();
			if (pd == ENUM_PLAYER_DIRECTION_DOWN)
				playerPP->setVelocity(0.0f, 40.0f);
			else if (pd == ENUM_PLAYER_DIRECTION_UP)
				playerPP->setVelocity(0.0f, -40.0f);
			else if (pd == ENUM_PLAYER_DIRECTION_LEFT)
				playerPP->setVelocity(-40.0f, 0.0f);
			else if (pd == ENUM_PLAYER_DIRECTION_RIGHT)
				playerPP->setVelocity(40.0f, 0.0f);
		}

		//// check if player sprite is going out of the world or not
		bool playerGoesOutWorld = doesSpriteGoOutWorldThisFrame(playerSprite);
		if(!playerGoesOutWorld)
			playerPP->update();
		

		//// --- bot physics update
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

//// doesSpriteGoOutWorld() 
//// return true if the sprite goes out of the world at this frame
bool Physics::doesSpriteGoOutWorldThisFrame(AnimatedSprite * sprite)
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	World *world = gsm->getWorld();
	float worldWidth = world->getWorldWidth();
	float worldHeight = world->getWorldHeight();

	PhysicalProperties *pp = sprite->getPhysicalProperties();
	float futureLeft = pp->getX() + pp->getVelocityX();
	float futureTop = pp->getY() + pp->getVelocityY();
	float futureRight = futureLeft + sprite->getSpriteType()->getTextureWidth();
	float futureBottom = futureTop + sprite->getSpriteType()->getTextureHeight();

	if (futureLeft < 0)
		return true;
	if (futureTop < 0)
		return true;
	if (futureRight >= worldWidth)
		return true;
	if (futureBottom >= worldHeight)
		return true;

	return false;
}
