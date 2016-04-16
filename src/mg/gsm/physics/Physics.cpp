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
				float playerRight = playerLeft + 64;
				float playerTop = playerPP->getY() + 80;
				float playerBottom = playerTop + 48;

				int leftColumn = collidableLayer->getColumnByX(playerLeft);
				int rightColumn = collidableLayer->getColumnByX(playerRight);
				int topRow = collidableLayer->getRowByY(playerTop);
				int bottomRow = collidableLayer->getRowByY(playerBottom);

				playerPP->setVelocity(0.0f, 0.0f);

				if (pd == ENUM_PLAYER_DIRECTION_DOWN)
				{
					int vY = playerSprite->getSpeed();
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

					list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
					list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
					while (botIterator != end)
					{
						Bot *bot = (*botIterator);
						PhysicalProperties *pp = bot->getPhysicalProperties();
						pp->update();
						float left = pp->getX();
						float top = pp->getY();
						float right = pp->getX() + 64;
						float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

						if (bot->getInjured() == false)
						{


							if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() <= top && playerBottomNextFrame >= top + 64)
							{
								if (bot->getInjured() == false)
									vY = 0.0f;
							}
						}

						playerPP->setVelocity(0.0f, vY);


						botIterator++;
					}
				}
				else if (pd == ENUM_PLAYER_DIRECTION_UP)
				{
					int vY = -playerSprite->getSpeed();
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

					list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
					list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
					while (botIterator != end)
					{
						Bot *bot = (*botIterator);
						PhysicalProperties *pp = bot->getPhysicalProperties();
						pp->update();
						float left = pp->getX();
						float top = pp->getY();
						float right = pp->getX() + 64;
						float bottom = pp->getY() + 128;

						if (bot->getInjured() == false)
						{


							if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() + 80 >= bottom && playerPP->getY() + vY <= bottom - 40)
							{
								if (bot->getInjured() == false)
									vY = 0.0f;
							}
						}

						playerPP->setVelocity(0.0f, vY);


						botIterator++;
					}
				}
				else if (pd == ENUM_PLAYER_DIRECTION_LEFT){
					int vX = -playerSprite->getSpeed();
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

					list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
					list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
					while (botIterator != end)
					{
						Bot *bot = (*botIterator);
						PhysicalProperties *pp = bot->getPhysicalProperties();
						pp->update();
						float left = pp->getX();
						float top = pp->getY();
						float right = pp->getX() + 64;
						float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

						if (bot->getInjured() == false)
						{


							if (((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() >= right-5 && playerLeftNextFrame <= right + 5)
							{
								if (bot->getInjured() == false)
									vX = 0.0f;
							}
						}

						playerPP->setVelocity(vX, 0.0f);


						botIterator++;
					}
				}
				else if (pd == ENUM_PLAYER_DIRECTION_RIGHT) {
					int vX = playerSprite->getSpeed();
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

					list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
					list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
					while (botIterator != end)
					{
						Bot *bot = (*botIterator);
						PhysicalProperties *pp = bot->getPhysicalProperties();
						pp->update();
						float left = pp->getX();
						float top = pp->getY();
						float right = pp->getX() + 64;
						float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

						if (bot->getInjured() == false)
						{


							if (((playerPP->getY() + 20 >= pp->getY() && playerPP->getY() + 20 <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() <= right - 5 && playerRightNextFrame >= right)
							{
								if (bot->getInjured() == false)
									vX = 0.0f;
							}
						}

						playerPP->setVelocity(vX, 0.0f);


						botIterator++;
					}
				}
			}

			if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_FRONT") == 0) {

					// CHECK TO SEE IF THE DYING ANIMATION IS DONE
					AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
					unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
					unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;
					if (frameIndex > sequenceSize) {
						if (playerSprite->getCurrentState().compare(L"SHOOT_FRONT") == 0) 
							gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
						if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0)
							punch(playerSprite, true, gsm->isSafetyon());
						playerSprite->setCurrentState(L"IDLE_FRONT");
					}
				
			}
			if (playerSprite->getCurrentState().compare(L"PUNCH_BACK") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_BACK") == 0) {

				// CHECK TO SEE IF THE DYING ANIMATION IS DONE
				AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
				unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
				unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;
				if (frameIndex > sequenceSize) {
					if (playerSprite->getCurrentState().compare(L"SHOOT_BACK") == 0)
						gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
					if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0)
						punch(playerSprite, true, gsm->isSafetyon());
					playerSprite->setCurrentState(L"IDLE_BACK");
				}

			}
			if (playerSprite->getCurrentState().compare(L"PUNCH_LEFT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_LEFT") == 0) {

				// CHECK TO SEE IF THE DYING ANIMATION IS DONE
				AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
				unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
				unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;
				if (frameIndex > sequenceSize) {
					if (playerSprite->getCurrentState().compare(L"SHOOT_LEFT") == 0)
						gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
					if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0)
						punch(playerSprite, true, gsm->isSafetyon());
					playerSprite->setCurrentState(L"IDLE_LEFT");
				}

			}
			if (playerSprite->getCurrentState().compare(L"PUNCH_RIGHT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_RIGHT") == 0) {

				// CHECK TO SEE IF THE DYING ANIMATION IS DONE
				AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
				unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
				unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;
				if (frameIndex > sequenceSize) {
					if (playerSprite->getCurrentState().compare(L"SHOOT_RIGHT") == 0)
						gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
					if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0)
						punch(playerSprite, true, gsm->isSafetyon());
					playerSprite->setCurrentState(L"IDLE_RIGHT");
				}

			}

			//// check if player sprite is going out of the world or not
			bool playerGoesOutWorld = doesSpriteGoOutWorldThisFrame(playerSprite);
			if(!playerGoesOutWorld)
				playerPP->update();

		}


		//// --- bot physics update
		/// here
		if (botActivated) 
		{
			PlayerSprite *playerSprite = spriteManager->getPlayer();
			PhysicalProperties *playerPP = playerSprite->getPhysicalProperties();

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

				if (bot->getInjured() == false)
				{

					float BottomNextFrame = bottom + playerPP->getVelocityY();

					if (playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64
						&& playerPP->getY() <= pp->getY() + 128 && playerPP->getY() >= pp->getY())
					{
						if (bot->getInjured() == false)
							playerPP->setVelocity(0.0f, 0.0f);
					}
				}


				botIterator++;
			}
		}
		bool fired = false;
		list<LevelObjectSprite*>::iterator itemIterator = spriteManager->getLevelSpriteObjectsIterator();
		list<LevelObjectSprite*>::iterator end = spriteManager->getEndOfLevelSpriteObjectsIterator();
		while (itemIterator != end)
		{
			LevelObjectSprite *LevelObjectSprite = (*itemIterator);
			PhysicalProperties *pp = LevelObjectSprite->getPhysicalProperties();
			pp->update();
			float left = pp->getX();
			float top = pp->getY();
			float right = pp->getX() + LevelObjectSprite->getSpriteType()->getTextureWidth();
			float bottom = pp->getY() + LevelObjectSprite->getSpriteType()->getTextureHeight();

			TiledLayer *collidableLayer = world->getCollidableLayer();

			int leftColumn = collidableLayer->getColumnByX(left);
			int rightColumn = collidableLayer->getColumnByX(right);
			int topRow = collidableLayer->getRowByY(top);
			int bottomRow = collidableLayer->getRowByY(bottom);

			float vX = pp->getVelocityX();
			float vY = pp->getVelocityY();

			if (vY > 0)
			{
				float BottomNextFrame = bottom + vY;

				if (BottomNextFrame < world->getWorldHeight())
				{
					int bottomRowNextFrame = collidableLayer->getRowByY(BottomNextFrame);

					for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
					{
						Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
						if (tile->collidable)
						{
							vY = 0.0f;
							break;
						}
					}

					
				}
			}
			else if (vY < 0)
			{
				float TopNextFrame = top + vY;

				if (TopNextFrame > 0.0f) {
					int topRowNextFrame = collidableLayer->getRowByY(TopNextFrame);

					for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++) {
						Tile *tile = collidableLayer->getTile(topRowNextFrame, columnIndex);
						if (tile->collidable)
						{
							vY = 0.0f;
							break;
						}
					}
					pp->setVelocity(0.0f, vY);
				}
			}
			else if (vX > 0)
			{
				float RightNextFrame = right + vX;

				if (RightNextFrame > 0.0f) {
					int rightColumnNextFrame = collidableLayer->getColumnByX(RightNextFrame);

					for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
						Tile *tile = collidableLayer->getTile(rowIndex, rightColumnNextFrame);
						if (tile->collidable)
						{
							vX = 0.0f;
							break;
						}
					}
					pp->setVelocity(vX, 0.0f);
				}
			}
			else if (vX < 0)
			{
				float LeftNextFrame = left + vX;

				if (LeftNextFrame > 0.0f) {
					int leftColumnNextFrame = collidableLayer->getColumnByX(LeftNextFrame);

					for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
						Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);
						if (tile->collidable)
						{
							vX = 0.0f;
							break;
						}
					}
					pp->setVelocity(vX, 0.0f);
				}
			}

			if (LevelObjectSprite->getPlayer())
			{


				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();

				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *bpp = bot->getPhysicalProperties();

					if (pp->getX() >= bpp->getX() - 10 && pp->getX() <= bpp->getX() + 74
						&& pp->getY() <= bpp->getY() + 138 && pp->getY() >= bpp->getY() - 10)
					{
						spriteManager->removeLevelObject(LevelObjectSprite);
						fired = true;
						if (LevelObjectSprite->getSafetyon())
							bot->setCurrentState(L"DIE");
						else
							game->quitGame();
						break;
					}

					botIterator++;
				}
			}

			
			if (fired)
				break;

			itemIterator++;
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

void Physics::punch(AnimatedSprite *sprite, bool player, bool safety)
{
	Game *game = Game::getSingleton();
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	GameStateManager *gsm = game->getGSM();

	if (sprite == spriteManager->getPlayer())
	{

	}
}