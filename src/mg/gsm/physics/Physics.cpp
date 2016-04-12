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
#include "mg\gsm\world\World.h"
#include "mg\gsm\world\TiledLayer.h"

void Physics::startUp()
{
	activated = true;
	botActivated = true;
	playerActivated = true;
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

		/// here
		if(playerActivated)
		{
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

				World *world = gsm->getWorld();
				TiledLayer *collidableLayer = world->getCollidableLayer();
				int playerSpriteWidth = playerSprite->getSpriteType()->getTextureWidth();
				int playerSpriteHeight = playerSprite->getSpriteType()->getTextureHeight();

				float playerLeft = playerPP->getX();
				float playerRight = playerLeft + playerSpriteWidth;
				float playerTop = playerPP->getY();
				float playerBottom = playerTop + playerSpriteHeight;

				int leftColumn = collidableLayer->getColumnByX(playerLeft);
				int rightColumn = collidableLayer->getColumnByX(playerRight);
				int topRow = collidableLayer->getRowByY(playerTop);
				int bottomRow = collidableLayer->getRowByY(playerBottom);

				if (pd == ENUM_PLAYER_DIRECTION_DOWN)
				{
					float vY = MG_DEFAULT_PLAYER_SPEED;
					float playerBottomNextFrame = playerBottom + vY;
				
					if(playerBottomNextFrame < world->getWorldHeight()){
						int bottomRowNextFrame = collidableLayer->getRowByY(playerBottomNextFrame);
				
						for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
						{
							Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
							if (tile->collidable)
							{
								vY = 0.0f;
								break;
							}
						}
						playerPP->setVelocity(0.0f, vY);
					}
				}
				else if (pd == ENUM_PLAYER_DIRECTION_UP)
				{
					float vY = -MG_DEFAULT_PLAYER_SPEED;
					float playerTopNextFrame = playerTop + vY;

					if(playerTopNextFrame > 0.0f){
						int topRowNextFrame = collidableLayer->getRowByY(playerTopNextFrame);

						for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++){
							Tile *tile = collidableLayer->getTile(topRowNextFrame, columnIndex);
							if (tile->collidable)
							{
								vY = 0.0f;
								break;
							}
						}
						playerPP->setVelocity(0.0f, vY);
					}
				}
				else if (pd == ENUM_PLAYER_DIRECTION_LEFT){
					float vX = -MG_DEFAULT_PLAYER_SPEED;
					float playerLeftNextFrame = playerLeft + vX;

					if (playerLeftNextFrame > 0.0f) {
						int leftColumnNextFrame = collidableLayer->getColumnByX(playerLeftNextFrame);

						for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
							Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);
							if (tile->collidable)
							{
								vX = 0.0f;
								break;
							}
						}
						playerPP->setVelocity(vX, 0.0f);
					}			
				}
				else if (pd == ENUM_PLAYER_DIRECTION_RIGHT) {
					float vX = MG_DEFAULT_PLAYER_SPEED;
					float playerRightNextFrame = playerRight + vX;

					if (playerRightNextFrame > 0.0f) {
						int rightColumnNextFrame = collidableLayer->getColumnByX(playerRightNextFrame);

						for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
							Tile *tile = collidableLayer->getTile(rowIndex, rightColumnNextFrame);
							if (tile->collidable)
							{
								vX = 0.0f;
								break;
							}
						}
						playerPP->setVelocity(vX, 0.0f);
					}
				}
			}

			//// check if player sprite is going out of the world or not
			bool playerGoesOutWorld = doesSpriteGoOutWorldThisFrame(playerSprite);
			if(!playerGoesOutWorld)
				playerPP->update();
		}
		/// here

		//// --- bot physics update
		/// here
		if (botActivated) 
		{
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
		//// here
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
