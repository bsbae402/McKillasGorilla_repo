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
#include "rebelle\RebelleTextGenerator.h"
#include "mg\gsm\sprite\LevelObjectSprite.h"
#include "mg\platforms\DirectX\GameAudio.h"


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
		playerActivated = true;

		Game *game = Game::getSingleton();
		SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
		GameStateManager *gsm = game->getGSM();
		World *world = gsm->getWorld();
		TextGenerator *generator = game->getText()->getTextGenerator();

		exit = 0;

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

				if (playerSprite->getCurrentState().compare(L"SHOOT_LEFT") != 0
					&& playerSprite->getCurrentState().compare(L"SHOOT_RIGHT") != 0
					&& playerSprite->getCurrentState().compare(L"SHOOT_FRONT") != 0
					&& playerSprite->getCurrentState().compare(L"SHOOT_BACK") != 0)
				{

					if (pd == ENUM_PLAYER_DIRECTION_DOWN)
					{
						int vY = playerSprite->getSpeed();
						float playerBottomNextFrame = playerBottom + vY;

						if (playerBottomNextFrame < world->getWorldHeight()) {
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
							//pp->update();
							float left = pp->getX();
							float top = pp->getY();
							float right = pp->getX() + 64;
							float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

							//Check that the player does not move through bots that are not injured
							if (bot->getInjured() == false && ((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() <= top && playerBottomNextFrame >= top + 64)
							{
								vY = 0.0f;
							}
							//If a bot is injured, the player can heal him
							else if (bot->getInjured() == true && ((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() - 64 <= bottom + 10 && playerPP->getY() + 64 >= top - 10)
							{
								generator->setHeal(L"HEAL");
								playerSprite->setCanheal(true);

								//Heal the bot (Set to dying for testing)
								if (playerSprite->getIshealing() == true && bot->getInjured() == true)
								{
									bot->setPreviousState(bot->getCurrentState());
									bot->setCurrentState(L"IDLE_BACK");
									bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
									bot->setInjured(false);
									playerSprite->setIshealing(false);

									if (bot->getStartinjured() == true)
										gsm->setScore(gsm->getScore() + 1000);
									else
										gsm->setScore(gsm->getScore() + 500);

									bot->setHealth(bot->getStarthealth());
									bot->setWasHealed(true);
								}
							}
							//Set toolbar heal caption to blank
							else if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerSprite->getCanheal() == true && playerPP->getY() >= bottom)
							{

								generator->setHeal(L"");
								playerSprite->setCanheal(false);
							}


							playerPP->setVelocity(0.0f, vY);


							botIterator++;
						}
					}
					else if (pd == ENUM_PLAYER_DIRECTION_UP)
					{
						int vY = -playerSprite->getSpeed();
						float playerTopNextFrame = playerTop + vY;

						if (playerTopNextFrame > 0.0f) {
							int topRowNextFrame = collidableLayer->getRowByY(playerTopNextFrame);

							for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++) {
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
							//pp->update();
							float left = pp->getX();
							float top = pp->getY();
							float right = pp->getX() + 64;
							float bottom = pp->getY() + 128;


							//Check that the player does not move through bots that are not injured
							if (bot->getInjured() == false && ((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() + 80 >= bottom && playerPP->getY() + vY <= bottom - 40)
							{
								vY = 0.0f;

							}
							//If a bot is injured, the player can heal him
							else if (bot->getInjured() == true && ((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerPP->getY() + 64 <= bottom + 10 && playerPP->getY() >= top - 10)
							{
								generator->setHeal(L"HEAL");
								playerSprite->setCanheal(true);

								//Heal the bot (Set to dying for testing)
								if (playerSprite->getIshealing() == true && bot->getInjured() == true)
								{
									bot->setPreviousState(bot->getCurrentState());
									bot->setCurrentState(L"IDLE_BACK");
									bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
									bot->setInjured(false);
									playerSprite->setIshealing(false);

									if (bot->getStartinjured() == true)
										gsm->setScore(gsm->getScore() + 1000);
									else
										gsm->setScore(gsm->getScore() + 500);

									bot->setHealth(bot->getStarthealth());
									bot->setWasHealed(true);
								}
							}
							//Set toolbar heal caption to blank
							else if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
								|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
								&& playerSprite->getCanheal() == true && playerPP->getY() <= top)
							{
								int x = playerPP->getX();
								generator->setHeal(L"");
								playerSprite->setCanheal(false);
							}


							playerPP->setVelocity(0.0f, vY);


							botIterator++;
						}
					}
					else if (pd == ENUM_PLAYER_DIRECTION_LEFT) {
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
							//pp->update();
							float left = pp->getX();
							float top = pp->getY();
							float right = pp->getX() + 64;
							float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();


							//Check that the player does not move through bots that are not injured
							if (bot->getInjured() == false && ((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() >= right - 5 && playerLeftNextFrame <= right + 5)
							{
								vX = 0.0f;

							}
							//If a bot is injured, the player can heal him
							else if (bot->getInjured() == true && ((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() >= left - 10 && playerPP->getX() <= right + 10)
							{
								generator->setHeal(L"HEAL");
								playerSprite->setCanheal(true);

								//Heal the bot (Set to dying for testing)
								if (playerSprite->getIshealing() == true && bot->getInjured() == true)
								{
									bot->setPreviousState(bot->getCurrentState());
									bot->setCurrentState(L"IDLE_BACK");
									bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
									bot->setInjured(false);
									playerSprite->setIshealing(false);

									if (bot->getStartinjured() == true)
										gsm->setScore(gsm->getScore() + 1000);
									else
										gsm->setScore(gsm->getScore() + 500);

									bot->setHealth(bot->getStarthealth());

									bot->setWasHealed(true);
									pp->setX(pp->getX() + 50);
								}
							}
							//Set toolbar heal caption to blank
							else if (((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerSprite->getCanheal() == true && playerPP->getX() <= left)
							{
								int x = playerPP->getX();
								generator->setHeal(L"");
								playerSprite->setCanheal(false);
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
							//pp->update();
							float left = pp->getX();
							float top = pp->getY();
							float right = pp->getX() + 64;
							float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();
							
							//Check that the player does not move through bots that are not injured
							if (bot->getInjured() == false && ((playerPP->getY() + 20 >= pp->getY() && playerPP->getY() + 20 <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() <= right - 5 && playerRightNextFrame >= right)
							{
								vX = 0.0f;

							}
							//If a bot is injured, the player can heal him
							else if (bot->getInjured() == true && ((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerPP->getX() >= left - 10 && playerPP->getX() <= right + 10)
							{
								generator->setHeal(L"HEAL");
								playerSprite->setCanheal(true);

								//Heal the bot (Set to dying for testing)
								if (playerSprite->getIshealing() == true && bot->getInjured() == true)
								{
									bot->setPreviousState(bot->getCurrentState());
									bot->setCurrentState(L"IDLE_BACK");
									bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
									bot->setInjured(false);
									playerSprite->setIshealing(false);

									if (bot->getStartinjured() == true)
										gsm->setScore(gsm->getScore() + 1000);
									else
										gsm->setScore(gsm->getScore() + 500);

									bot->setHealth(bot->getStarthealth());

									bot->setWasHealed(true);
								}
							}
							//Set toolbar heal caption to blank
							else if (((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
								|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
								&& playerSprite->getCanheal() == true && playerPP->getX() >= right)
							{
								int x = playerPP->getX();
								generator->setHeal(L"");
								playerSprite->setCanheal(false);
							}


							playerPP->setVelocity(vX, 0.0f);


							botIterator++;
						}
					}
				}
			}

			if (playerPP->getX() >= gsm->getEndlevelx() && playerPP->getX() <= gsm->getEndlevelxwidth()
				&& playerPP->getY() >= gsm->getEndlevely() && playerPP->getY() <= gsm->getEndlevelywidth())
			{
				//game->quitGame();
				//exit = 1;
				gsm->goToLevelComplete();
			}

			if (exit == 0)
			{
				if (gsm->getSpriteManager()->getPlayer()->getCurrentState().compare(L"DYING") != 0
					&& gsm->getSpriteManager()->getPlayer()->getCurrentState().compare(L"DEAD") != 0)
				{
					//// check if player sprite is going out of the world or not
					bool playerGoesOutWorld = doesSpriteGoOutWorldThisFrame(playerSprite);
					if (!playerGoesOutWorld)
						playerPP->update();
				}
			}
			

		}

		
		
		//// --- bot physics update
		/// here
		if (exit == 0)
		{
			//if (botActivated)
			{
				PlayerSprite *playerSprite = spriteManager->getPlayer();
				PhysicalProperties *playerPP = playerSprite->getPhysicalProperties();

				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *pp = bot->getPhysicalProperties();
					//pp->update();
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



						if (playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64
							&& playerPP->getY() <= pp->getY() + 128 && playerPP->getY() >= pp->getY())
						{
							if (bot->getInjured() == false)
							{
								playerPP->setVelocity(0.0f, 0.0f);
							}
						}
					}
					else
						bot->setCurrentState(L"DEAD");

					if (bot->getCurrentState().compare(L"DYING") == 0)
					{
						unsigned int frameIndex = (bot->getFrameIndex() * 2);
						unsigned int sequenceSize = bot->getSpriteType()->getSequenceSize(bot->getCurrentState()) + 2;

						if (frameIndex >= sequenceSize) {
							bot->setPreviousState(bot->getCurrentState());
							bot->setCurrentState(L"DEAD");
						}
					}


					botIterator++;
				}
			}
		

			CheckPunchShoot(spriteManager->getPlayer());
			CheckDying(spriteManager->getPlayer());
			GameOverCountDown();
			GoToUpgrades();
			
			if (exit == 0)
			{

				bool used = false; //Check for if we are removing an item to get out of the list loop
				list<LevelObjectSprite*>::iterator itemIterator = spriteManager->getLevelSpriteObjectsIterator();
				list<LevelObjectSprite*>::iterator end = spriteManager->getEndOfLevelSpriteObjectsIterator();
				while (itemIterator != end)
				{
					LevelObjectSprite *los = (*itemIterator);
					PhysicalProperties *pp = los->getPhysicalProperties();
					pp->update();
					float left = pp->getX();

					float top = pp->getY();
					float right = pp->getX() + los->getSpriteType()->getTextureWidth();
					float bottom = pp->getY() + los->getSpriteType()->getTextureHeight();

					TiledLayer *collidableLayer = world->getCollidableLayer();

					int leftColumn = collidableLayer->getColumnByX(left);
					int rightColumn = collidableLayer->getColumnByX(right);
					int topRow = collidableLayer->getRowByY(top);
					int bottomRow = collidableLayer->getRowByY(bottom);

					float vX = pp->getVelocityX();
					float vY = pp->getVelocityY();

					//Remove money and add to money count if a player moves into money sprite
					if (los->getType().compare(L"money") == 0)
					{
						PhysicalProperties *playerpp = spriteManager->getPlayer()->getPhysicalProperties();

						if ((playerpp->getX() >= pp->getX() && playerpp->getX() <= pp->getX() + 64
							|| playerpp->getX() + 64 >= pp->getX() && playerpp->getX() + 64 <= pp->getX() + 64)
							&& (playerpp->getY() >= pp->getY() && playerpp->getY() <= pp->getY() + 64
								|| playerpp->getY() + 128 >= pp->getY() && playerpp->getY() + 128 <= pp->getY() + 64
								|| playerpp->getY() <= pp->getY() && playerpp->getY() + 128 >= pp->getY() + 64))
						{
							GameAudio *audio = game->getAudio();
							audio->recieveMoneySoundSignal();

							spriteManager->removeLevelObject(los);
							gsm->setMoney(gsm->getMoney() + 500);
							used = true;
							break;
						}
						itemIterator++;
						continue;
					}

					//Check that a bullet is removed if it hits a wall
					if (vY > 0)
					{
						float BottomNextFrame = bottom + vY;

						if (BottomNextFrame < world->getWorldHeight()) {
							int bottomRowNextFrame = collidableLayer->getRowByY(BottomNextFrame);

							for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
							{
								Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
								
								if (tile->collidable)
								{
									spriteManager->removeLevelObject(los);
									vY = 0.0f;
									used = true;
									break;
								}
							}
							pp->setVelocity(0.0f, vY);
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
									spriteManager->removeLevelObject(los);
									vY = 0.0f;
									used = true;
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
									spriteManager->removeLevelObject(los);
									vX = 0.0f;
									used = true;
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
									spriteManager->removeLevelObject(los);
									vX = 0.0f;
									used = true;
									break;
								}
								
							}
							pp->setVelocity(vX, 0.0f);
						}
					}

					

					//Check for if a bullet came from the player. Bullets from bots cannot kill a bot
					if (los->getPlayer())
					{


						list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
						list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();

						while (botIterator != end)
						{
							Bot *bot = (*botIterator);
							PhysicalProperties *bpp = bot->getPhysicalProperties();

							//Can only shoot a bot that is not injured
							if (bot->getInjured() == false)
							{
								if (pp->getX() >= bpp->getX() - 10 && pp->getX() <= bpp->getX() + 74
									&& pp->getY() <= bpp->getY() + 138 && pp->getY() >= bpp->getY() - 10)
								{
									spriteManager->removeLevelObject(los);
									used = true;

									//If the safety is on, set bot to injured (Dying is the animation). Otherwise 
									//it's an automatic game over
									if (los->getSafetyon())
									{
										bot->setPreviousState(bot->getCurrentState());
										bot->setCurrentState(L"DYING");
										bot->setInjured(true);

										if (bot->getStartinjured() == true && bot->getWasHealed() == true)
											gsm->setScore(gsm->getScore() - 1000);
										else if (bot->getStartinjured() == false && bot->getWasHealed() == true)
											gsm->setScore(gsm->getScore() - 500);
									}
									else
									{
										bot->setCurrentState(L"DYING");
										bot->setInjured(true);
										gameover = true;
									}
									break;
								}
							}

							botIterator++;
						}
					}
					else
					{
						PhysicalProperties *playerpp = spriteManager->getPlayer()->getPhysicalProperties();

						//Can only shoot a bot that is not injured
						//if (bot->getInjured() == false)
						{
							if (pp->getX() >= playerpp->getX() - 10 && pp->getX() <= playerpp->getX() + 74
								&& pp->getY() <= playerpp->getY() + 138 && pp->getY() >= playerpp->getY() - 10)
							{
								spriteManager->removeLevelObject(los);
								used = true;

								//If the safety is on, set bot to injured (Dying is the animation). Otherwise 
								//it's an automatic game over
								//if (LevelObjectSprite->getSafetyon())
								{
									//bot->setPreviousState(bot->getCurrentState());
									//bot->setCurrentState(L"DYING");
									//bot->setInjured(true);
									if (spriteManager->getPlayer()->isInvincible() == false && (spriteManager->getPlayer()->getCurrentState().compare(L"DEAD") != 0 &&
										spriteManager->getPlayer()->getCurrentState().compare(L"DYING") != 0))
									{
										if (spriteManager->getPlayer()->getDefense() >= los->getAttack())
											spriteManager->getPlayer()->setHealth(spriteManager->getPlayer()->getHealth() - 0);
										else
											spriteManager->getPlayer()->setHealth(spriteManager->getPlayer()->getHealth() - los->getAttack() + spriteManager->getPlayer()->getDefense());
										if (spriteManager->getPlayer()->getPlayerDirection() == ENUM_PLAYER_DIRECTION_DOWN)
										{
											spriteManager->getPlayer()->setCurrentState(L"DAMAGE_BACK");
										}
										else if (spriteManager->getPlayer()->getPlayerDirection() == ENUM_PLAYER_DIRECTION_UP)
										{
											spriteManager->getPlayer()->setCurrentState(L"DAMAGE_FRONT");
										}
										else if (spriteManager->getPlayer()->getPlayerDirection() == ENUM_PLAYER_DIRECTION_LEFT)
										{
											spriteManager->getPlayer()->setCurrentState(L"DAMAGE_LEFT");
										}
										else if (spriteManager->getPlayer()->getPlayerDirection() == ENUM_PLAYER_DIRECTION_RIGHT)
										{
											spriteManager->getPlayer()->setCurrentState(L"DAMAGE_RIGHT");
										}
									}

									gsm->setMoveviewport(false);
								}
								if (spriteManager->getPlayer()->getHealth() <= 0 &&
									(spriteManager->getPlayer()->getCurrentState().compare(L"DEAD") != 0 &&
										spriteManager->getPlayer()->getCurrentState().compare(L"DYING") != 0))
								{
									playerActivated = false;
									gameover = true;
									spriteManager->getPlayer()->setCurrentState(L"DYING");
									exit = 1;
								}
								
							}
						}
					}

					if (used)
					{
						delete los;
						break;
					}
					itemIterator++;
				}	//// <- end of while loop of itemIterator(los)

				used = false;
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
	//float futureTop = pp->getY() + pp->getVelocityY();
	//float futureRight = futureLeft + sprite->getSpriteType()->getTextureWidth();
	//float futureBottom = futureTop + sprite->getSpriteType()->getTextureHeight();
	/*	HERE ARE WHAT WE ARE USING IN THE PHYSICS UPDATE ...
				float playerLeft = playerPP->getX();
				float playerRight = playerLeft + 64;
				float playerTop = playerPP->getY() + 80;
				float playerBottom = playerTop + 48;
	*/
	float futureTop = pp->getY() + pp->getVelocityY() + 80;
	float futureRight = futureLeft + 64;
	float futureBottom = futureTop + 48;

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
		PlayerSprite *playerSprite = spriteManager->getPlayer();
		PhysicalProperties *playerPP = playerSprite->getPhysicalProperties();

		//// set velocity of player
		PlayerState ps = playerSprite->getPlayerState();
		
		
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

				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *pp = bot->getPhysicalProperties();
					//pp->update();
					float left = pp->getX();
					float top = pp->getY();
					float right = pp->getX() + 64;
					float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

					if (bot->getInjured() == false)
					{

						//Check if a player punches an enemy with the safety off (Set to quit game for testing)
						if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
							|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
							&& playerPP->getY() <= top && playerBottomNextFrame >= top + 64)
						{
							if (punched == false)
							{
								if (safety == false)
									gsm->setScore(gsm->getScore() - 100);
								bot->setHealth(bot->getHealth() - playerSprite->getAttack() + bot->getDefense());
								punched = true;
							}
							exit = 1;
							CheckPunchShoot(bot);

							if (bot->getHealth() <= 0 && safety == true)
							{
								bot->setHealth(0);
								bot->setPreviousState(bot->getCurrentState());
								bot->setCurrentState(L"DYING");
								bot->setInjured(true);
								if (bot->getStartinjured() == true && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 1000);
								else if (bot->getStartinjured() == false && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 500);
							}
							else if (bot->getHealth() <= 0 && safety == false)
							{
								bot->setHealth(0);
								bot->setInjured(true);
								bot->setCurrentState(L"DYING");
								gameover = true;
							}
							break;
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


				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *pp = bot->getPhysicalProperties();
					//pp->update();
					float left = pp->getX();
					float top = pp->getY();
					float right = pp->getX() + 64;
					float bottom = pp->getY() + 128;

					if (bot->getInjured() == false)
					{

						//Check if a player punches an enemy with the safety off (Set to quit game for testing)
						if (((playerPP->getX() >= pp->getX() && playerPP->getX() <= pp->getX() + 64)
							|| (playerPP->getX() + 64 >= pp->getX() && playerPP->getX() + 64 <= pp->getX() + 64))
							&& playerPP->getY() + 80 >= bottom && playerPP->getY() + vY <= bottom - 40)
						{
							if (punched == false)
							{
								if (safety == false)
									gsm->setScore(gsm->getScore() - 100);
								bot->setHealth(bot->getHealth() - playerSprite->getAttack() + bot->getDefense());
								punched = true;
							}
							exit = 1;
							CheckPunchShoot(bot);

							if (bot->getHealth() <= 0 && safety == true)
							{
								bot->setHealth(0);
								bot->setPreviousState(bot->getCurrentState());
								bot->setCurrentState(L"DYING");
								bot->setInjured(true);
								if (bot->getStartinjured() == true && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 1000);
								else if (bot->getStartinjured() == false && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 500);
							}
							else if (bot->getHealth() <= 0 && safety == false)
							{
								bot->setHealth(0);
								bot->setInjured(true);
								bot->setCurrentState(L"DYING");
								gameover = true;
							}
							
							break;
						}
					}

					playerPP->setVelocity(0.0f, vY);


					botIterator++;
				}
			}
			else if (pd == ENUM_PLAYER_DIRECTION_LEFT) {
				int vX = -playerSprite->getSpeed();
				float playerLeftNextFrame = playerLeft + vX;


				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *pp = bot->getPhysicalProperties();
					//pp->update();
					float left = pp->getX();
					float top = pp->getY();
					float right = pp->getX() + 64;
					float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

					if (bot->getInjured() == false)
					{

						//Check if a player punches an enemy with the safety off (Set to quit game for testing)
						if (((playerPP->getY() >= pp->getY() && playerPP->getY() <= pp->getY() + 128)
							|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
							&& playerPP->getX() >= right - 5 && playerLeftNextFrame <= right + 5)
						{

								//game->quitGame();
							if (punched == false)
							{
								if(safety == false)
									gsm->setScore(gsm->getScore() - 100);
								bot->setHealth(bot->getHealth() - playerSprite->getAttack() + bot->getDefense());
								punched = true;
							}
							exit = 1;
							CheckPunchShoot(bot);

							if (bot->getHealth() <= 0 && safety == true)
							{
								bot->setHealth(0);
								bot->setPreviousState(bot->getCurrentState());
								bot->setCurrentState(L"DYING");
								bot->setInjured(true);
								if (bot->getStartinjured() == true && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 1000);
								else if (bot->getStartinjured() == false && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 500);
							}
							else if (bot->getHealth() <= 0 && safety == false)
							{
								bot->setHealth(0);
								bot->setInjured(true);
								bot->setCurrentState(L"DYING");
								gameover = true;
								exit = 1;
							}
							

							
							
							break;
						}
					}

					playerPP->setVelocity(vX, 0.0f);


					botIterator++;
				}
			}
			else if (pd == ENUM_PLAYER_DIRECTION_RIGHT) {
				int vX = playerSprite->getSpeed();
				float playerRightNextFrame = playerRight + vX;


				list<Bot*>::iterator botIterator = spriteManager->getBotsIterator();
				list<Bot*>::iterator end = spriteManager->getEndOfBotsIterator();
				while (botIterator != end)
				{
					Bot *bot = (*botIterator);
					PhysicalProperties *pp = bot->getPhysicalProperties();
					//pp->update();
					float left = pp->getX();
					float top = pp->getY();
					float right = pp->getX() + 64;
					float bottom = pp->getY() + bot->getSpriteType()->getTextureHeight();

					if (bot->getInjured() == false)
					{

						//Check if a player punches an enemy with the safety off (Set to quit game for testing)
						if (((playerPP->getY() + 20 >= pp->getY() && playerPP->getY() + 20 <= pp->getY() + 128)
							|| (playerPP->getY() + 128 >= pp->getY() && playerPP->getY() + 128 <= pp->getY() + 128))
							&& playerPP->getX() <= right - 5 && playerRightNextFrame >= right)
						{
							
							if (punched == false)
							{
								if (safety == false)
									gsm->setScore(gsm->getScore() - 100);
								bot->setHealth(bot->getHealth() - playerSprite->getAttack() + bot->getDefense());
								punched = true;
							}
							exit = 1;
							CheckPunchShoot(bot);

							if (bot->getHealth() <= 0 && safety == true)
							{
								bot->setHealth(0);
								bot->setPreviousState(bot->getCurrentState());
								bot->setCurrentState(L"DYING");
								bot->setInjured(true);
								if (bot->getStartinjured() == true && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 1000);
								else if (bot->getStartinjured() == false && bot->getWasHealed() == true)
									gsm->setScore(gsm->getScore() - 500);
							}
							else if (bot->getHealth() <= 0 && safety == false)
							{
								bot->setHealth(0);
								bot->setCurrentState(L"DYING");
								bot->setInjured(true);
								gameover = true;
								exit = 1;
							}
							break;
						}
					}

					playerPP->setVelocity(vX, 0.0f);


					botIterator++;
				}
			}
		
	}
}

void Physics::CheckPunchShoot(AnimatedSprite *playerSprite)
{
	Game *game = Game::getSingleton();
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	GameStateManager *gsm = game->getGSM();


	if (playerSprite->getCurrentState().compare(L"PUNCH_LEFT") != 0
		&& playerSprite->getCurrentState().compare(L"PUNCH_RIGHT") != 0
		&& playerSprite->getCurrentState().compare(L"PUNCH_FRONT") != 0
		&& playerSprite->getCurrentState().compare(L"PUNCH_BACK") != 0)
		punched = false;

	if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_FRONT") == 0 || playerSprite->getCurrentState().compare(L"DAMAGE_FRONT") == 0) {

		// CHECK TO SEE IF THE DYING ANIMATION IS DONE
		AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
		unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
		unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;

		if (playerSprite->getCurrentState().compare(L"PUNCH_FRONT") == 0)
			punch(playerSprite, true, gsm->isSafetyon());


		if (frameIndex >= sequenceSize) {
			if (playerSprite->getCurrentState().compare(L"SHOOT_FRONT") == 0)
				gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
			playerSprite->setPreviousState(playerSprite->getCurrentState());
			playerSprite->setCurrentState(L"IDLE_FRONT");
		}

	}
	else if (playerSprite->getCurrentState().compare(L"PUNCH_BACK") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_BACK") == 0 || playerSprite->getCurrentState().compare(L"DAMAGE_BACK") == 0) {

		// CHECK TO SEE IF THE DYING ANIMATION IS DONE
		AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
		unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
		unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;

		if (playerSprite->getCurrentState().compare(L"PUNCH_BACK") == 0)
			punch(playerSprite, true, gsm->isSafetyon());

		if (frameIndex >= sequenceSize) {
			if (playerSprite->getCurrentState().compare(L"SHOOT_BACK") == 0)
				gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
			playerSprite->setPreviousState(playerSprite->getCurrentState());
			playerSprite->setCurrentState(L"IDLE_BACK");
		}

	}
	else if (playerSprite->getCurrentState().compare(L"PUNCH_LEFT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_LEFT") == 0 || playerSprite->getCurrentState().compare(L"DAMAGE_LEFT") == 0) {


		// CHECK TO SEE IF THE DYING ANIMATION IS DONE
		AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
		unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
		unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;

		if (playerSprite->getCurrentState().compare(L"PUNCH_LEFT") == 0)
			punch(playerSprite, true, gsm->isSafetyon());
		else
			punched = false;

		if (frameIndex >= sequenceSize) {
			if (playerSprite->getCurrentState().compare(L"SHOOT_LEFT") == 0)
				gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
			playerSprite->setPreviousState(playerSprite->getCurrentState());
			playerSprite->setCurrentState(L"IDLE_LEFT");
		}

	}
	else if (playerSprite->getCurrentState().compare(L"PUNCH_RIGHT") == 0 || playerSprite->getCurrentState().compare(L"SHOOT_RIGHT") == 0 || playerSprite->getCurrentState().compare(L"DAMAGE_RIGHT") == 0) {

		// CHECK TO SEE IF THE DYING ANIMATION IS DONE
		AnimatedSpriteType *spriteType = playerSprite->getSpriteType();
		unsigned int frameIndex = (playerSprite->getFrameIndex() * 2);
		unsigned int sequenceSize = spriteType->getSequenceSize(playerSprite->getCurrentState()) + 2;

		if (playerSprite->getCurrentState().compare(L"PUNCH_RIGHT") == 0)
			punch(playerSprite, true, gsm->isSafetyon());

		if (frameIndex >= sequenceSize) {
			if (playerSprite->getCurrentState().compare(L"SHOOT_RIGHT") == 0)
				gsm->getSpriteManager()->fireBullet(playerSprite, true, gsm->isSafetyon());
			playerSprite->setPreviousState(playerSprite->getCurrentState());
			playerSprite->setCurrentState(L"IDLE_RIGHT");
		}

	}
}

void Physics::CheckDying(AnimatedSprite *sprite)
{

	if (sprite->getCurrentState().compare(L"DYING") == 0) {

		Game *game = Game::getSingleton();
		SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
		GameStateManager *gsm = game->getGSM();

		// CHECK TO SEE IF THE DYING ANIMATION IS DONE
		AnimatedSpriteType *spriteType = sprite->getSpriteType();
		unsigned int frameIndex = (sprite->getFrameIndex() * 2);
		unsigned int sequenceSize = spriteType->getSequenceSize(sprite->getCurrentState()) + 2;


		if (frameIndex > sequenceSize) {
			
			sprite->setCurrentState(L"DEAD");
			gameover = true;
		}

	}
}

void Physics::GameOverCountDown()
{
	if (gameover == true)
	{
		countdown--;
		if (countdown == false)
		{
			Game *game = Game::getSingleton();
			game->quitGame();
		}
	}
}

void Physics::GoToUpgrades()
{
	Game *game = Game::getSingleton();
	SpriteManager *spriteManager = game->getGSM()->getSpriteManager();
	GameStateManager *gsm = game->getGSM();
	TextGenerator *generator = game->getText()->getTextGenerator();

	list<Upgrade*>::iterator upgradeIterator = gsm->getUpgradesIterator();
	list<Upgrade*>::iterator end = gsm->getEndOfUpgradesIterator();
	while (upgradeIterator != end && !gsm->getUpgrades().empty())
	{
		//list<Upgrade*>::iterator upgradeIterators = gsm->getUpgrades().begin();
	//	gsm->getUpgrades();
		Upgrade *upgrade = (*upgradeIterator);

		if (upgrade->getActivated() == true)
		{
			//upgrade->setWasActivated(true);
			upgrade->setTime(upgrade->getTime() - 1);
			if (upgrade->getTime() <= 0)
			{
				if (upgrade->getType().compare(L"ATTACK") == 0)
				{
					generator->setdebug(L"ATTACK");
					game->getGSM()->getSpriteManager()->getPlayer()->setAttack(game->getGSM()->getSpriteManager()->getPlayer()->getAttack() - 1);
				}
				else if (upgrade->getType().compare(L"DEFENSE") == 0)
				{
					generator->setdebug(L"DEFENSE");
					game->getGSM()->getSpriteManager()->getPlayer()->setDefense(game->getGSM()->getSpriteManager()->getPlayer()->getDefense() - 2);

				}
				else if (upgrade->getType().compare(L"SPEED") == 0)
				{
					generator->setdebug(L"SPEED");
					game->getGSM()->getSpriteManager()->getPlayer()->setSpeed(game->getGSM()->getSpriteManager()->getPlayer()->getSpeed() - 2);

				}

				gsm->removeUpgrade(upgrade);
				break;
			}
		}
		

		upgradeIterator++;
	}
}