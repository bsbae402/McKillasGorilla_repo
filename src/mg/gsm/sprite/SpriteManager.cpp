/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	SpriteManager.cpp

	See SpriteManager.h for a class description.
*/

#pragma once
#include "mg_VS\stdafx.h"
#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\physics\PhysicalProperties.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\sprite\AnimatedSpriteType.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\input\GameInput.h"
#include "mg\gsm\sprite\BulletRecycler.h"
#include "mg\gsm\sprite\LevelObjectSprite.h"

/*
	addSpriteToRenderList - This method checks to see if the sprite
	parameter is inside the viewport. If it is, a RenderItem is generated
	for that sprite and it is added to the render list.
*/
void SpriteManager::addSpriteToRenderList(AnimatedSprite *sprite,
										  RenderList *renderList,
										  Viewport *viewport)
{
	// GET THE SPRITE TYPE INFO FOR THIS SPRITE
	AnimatedSpriteType *spriteType = sprite->getSpriteType();
	PhysicalProperties *pp = sprite->getPhysicalProperties();
	float rotation = sprite->getRotationInRadians();

	// IS THE SPRITE VIEWABLE?
	if (viewport->areWorldCoordinatesInViewport(	
									pp->getX(),
									pp->getY(),
									spriteType->getTextureWidth(),
									spriteType->getTextureHeight()))
	{
		// SINCE IT'S VIEWABLE, ADD IT TO THE RENDER LIST
		//RenderItem itemToAdd;
		//itemToAdd.id = sprite->getFrameIndex();
		unsigned int currentImageId = sprite->getCurrentImageID();
		renderList->addRenderItem(	currentImageId,
									pp->round(pp->getX()-viewport->getViewportX()),
									pp->round(pp->getY()-viewport->getViewportY()),
									pp->round(pp->getZ()),
									sprite->getAlpha(),
									spriteType->getTextureWidth(),
									spriteType->getTextureHeight(),
									rotation);
	}
}

/*
	addSpriteItemsToRenderList - This method goes through all of the sprites,
	including the player sprite, and adds the visible ones to the render list.
	This method should be called each frame.
*/
void SpriteManager::addSpriteItemsToRenderList()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameGUI *gui = game->getGUI();
	if (gsm->isWorldRenderable())
	{
		GameGraphics *graphics = game->getGraphics();
		RenderList *renderList = graphics->getWorldRenderList();
		Viewport *viewport = gui->getViewport();

		// ADD THE PLAYER SPRITE, IF THERE IS ONE
		if (player != nullptr)
			addSpriteToRenderList(player, renderList, viewport);

		// NOW ADD THE REST OF THE SPRITES
		list<Bot*>::iterator botIterator;
		botIterator = bots.begin();
		while (botIterator != bots.end())
		{			
			Bot *bot = (*botIterator);
			addSpriteToRenderList(bot, renderList, viewport);
			botIterator++;
		}

		//// ---- add level objects (money, ...) to the render list ----
		list<LevelObjectSprite*>::iterator losIterator;
		losIterator = losList.begin();
		while (losIterator != losList.end())
		{
			LevelObjectSprite *los = (*losIterator);
			addSpriteToRenderList(los, renderList, viewport);
			losIterator++;
		}
		//// ---- end ----
	}
}

/*
	addSprite - This method is for adding a new sprite to 
	this sprite manager. Once a sprite is added it can be 
	scheduled for rendering.
*/
void SpriteManager::addBot(Bot *botToAdd)
{
	bots.push_back(botToAdd);
}

/*
	addSpriteType - This method is for adding a new sprite
	type. Note that one sprite type can have many sprites. For
	example, we may say that there may be a "Bunny" type of
	sprite, and specify properties for that type. Then there might
	be 100 different Bunnies each with their own properties, but that
	share many things in common according to what is defined in
	the shared sprite type object.
*/
unsigned int SpriteManager::addSpriteType(wstring spriteTypeName, AnimatedSpriteType *spriteTypeToAdd)
{
	spriteTypesByName[spriteTypeName] = spriteTypeToAdd;
	spriteTypes.push_back(spriteTypeToAdd);
	return spriteTypes.size()-1;
}

/*
	getSpriteType - This gets the sprite type object that corresponds
	to the index argument.
*/
AnimatedSpriteType* SpriteManager::getSpriteType(unsigned int typeIndex)
{
	if (typeIndex < spriteTypes.size())
		return spriteTypes.at(typeIndex);
	else
		return NULL;
}

/*
	getSpriteTypeByName - This gets the sprite type object that corresponds
	to the sprite name.
*/
AnimatedSpriteType* SpriteManager::getSpriteType(wstring spriteTypeName)
{
	return spriteTypesByName[spriteTypeName];
}

/*
	unloadSprites - This method removes all artwork from memory that
	has been allocated for game sprites.
*/
void SpriteManager::unloadSprites()
{
	// CLEAR OUT THE PLAYER, BUT NOT ITS SpriteType
	if (player != nullptr)
	{
		//// the problem was that the player sprite get deleted twice, which causes a runtime error.
		//// exit-> restart game process will call this unloadSprites() function twice.
		delete player;
		player = nullptr;
	}

	// CLEAR OUT THE BOTS, BUT NOT THEIR SpriteTypes
	list<Bot*>::iterator botsIt = bots.begin();
	while (botsIt != bots.end())
	{
		list<Bot*>::iterator tempIt = botsIt;
		botsIt++;
		Bot *botToDelete = (*tempIt);
		delete botToDelete;
	}
	bots.clear();

	////// --- delete level object sprites(money sprites)
	///// !! caution !! we need to unload these level objects seperately. 
	///// Because each of them has totally different ways of implementations since different team members do those codes. 
	//// First, I will unload the money sprites.
	/// bullet sprites are later.
	list<LevelObjectSprite*>::iterator losListIt = losList.begin();
	while (losListIt != losList.end())
	{
		list<LevelObjectSprite*>::iterator tempIt = losListIt;
		losListIt++;
		LevelObjectSprite *losToDelete = (*tempIt);
		if (losToDelete->getType().compare(L"money") == 0)
		{
			losList.remove(losToDelete);
			delete losToDelete;
		}
	}
	// don't clear the losList. because we still need to delete bullets. 

	//// <bullets clearing>
	//// I'm not sure I fully understand Steven's bulletRecycler implementation ....
	//// As I know, following bulletRecyler unloading will delete the actual bullet and the bullet "type"
	//// the bullet "type" is the one added to the losList during the MGLevelImporter's importing sequence.
	//// therefore, now we just need to clear out the losList, 
	//// because the bullet "type" los is deleted in the bulletRecycler's unload.
	bulletRecycler.unload();
	losList.clear();
	//// --- delete level object sprites complete

	// WE HAVE TO BE CAREFUL WITH SpriteTypes BECAUSE THEY ARE SHARED
	// BETWEEN SpriteManager AND BotRecycler, AS WELL AS EVERY Sprite.

	// LET'S CLEAR OUT AnimatedSpriteType FIRST, AND
	// THEN BE SURE NO CLASSES USING THEM DELETE THEM
	vector<AnimatedSpriteType*>::iterator spriteTypesIt = spriteTypes.begin();
	while (spriteTypesIt != spriteTypes.end())
	{
		vector<AnimatedSpriteType*>::iterator tempIt = spriteTypesIt;
		spriteTypesIt++;
		AnimatedSpriteType *astToDelete = (*tempIt);
		delete astToDelete;
	}
	spriteTypes.clear();
	spriteTypesByName.clear();

	// NOW LET'S UNLOAD THE BotRecycler, BUT BE CAREFUL
	// NOT TO DELETE THE AnimatedSpriteTypes SINCE WE
	// ALREADY DID SO
	botRecycler.unload();

	// AND FINALLY, THE SPAWNING POOLS
	vector<BotSpawningPool*>::iterator spawningPoolIt = spawningPools.begin();
	while (spawningPoolIt != spawningPools.end())
	{
		vector<BotSpawningPool*>::iterator tempIt = spawningPoolIt;
		spawningPoolIt++;
		BotSpawningPool *spToDelete = (*tempIt);
		delete spToDelete;
	}
	spawningPools.clear();
}

Bot* SpriteManager::removeBot(Bot *botToRemove)
{
	this->bots.remove(botToRemove);
	return botToRemove;
}

/*
	update - This method should be called once per frame. It
	goes through all of the sprites, including the player, and calls their
	update method such that they may update themselves.
*/
void SpriteManager::update()
{
	Game *game = Game::getSingleton();

	// UPDATE THE PLAYER SPRITE ANIMATION FRAME/STATE/ROTATION
	if (player != nullptr) {
		//// input check : player sprite remembers the input 
		//// so that we can track if the key is still down by GameInput class 
		GameInput *gameInput = game->getInput();

		int currentInputValue = player->getPlayerInputStorage();
		
		//// if input storage is empty
		if( currentInputValue == MG_NO_PLAYER_INPUT )
			player->updateSprite();

		//// if there is stored player input value
		else {
			//// check if key is still pressed by calling GameInput
			bool keyStillDown = gameInput->isKeyDown(currentInputValue);

			//// if key is finally not pressed (key up)
			if (!keyStillDown) {
				player->setPlayerInputStorage(MG_NO_PLAYER_INPUT);	// empty the input storage


				//// FOR NOW, if there is no key down, we force the player state to be idle.
				//// this seems to be bad implementation for our Jumping state, but that will be later.
				player->setPlayerState(ENUM_PLAYER_IDLE);	


				//// set player animation state idle according to the direction
				PlayerDirection pd = player->getPlayerDirection();

				if (!game->getGSM()->getSpriteManager()->getPlayer()->isStrafing())
				{
					if (pd == ENUM_PLAYER_DIRECTION_DOWN)
					{
						wstring wStrState(MG_PLAYER_ANIMATION_STATE_IDLE_BACK.begin(), MG_PLAYER_ANIMATION_STATE_IDLE_BACK.end());
						player->setPreviousState(player->getCurrentState());
						player->setCurrentState(wStrState);
					}
					else if (pd == ENUM_PLAYER_DIRECTION_UP)
					{
						wstring wStrState(MG_PLAYER_ANIMATION_STATE_IDLE_FRONT.begin(), MG_PLAYER_ANIMATION_STATE_IDLE_FRONT.end());
						player->setPreviousState(player->getCurrentState());
						player->setCurrentState(wStrState);
					}
					if (pd == ENUM_PLAYER_DIRECTION_LEFT)
					{
						wstring wStrState(MG_PLAYER_ANIMATION_STATE_IDLE_LEFT.begin(), MG_PLAYER_ANIMATION_STATE_IDLE_LEFT.end());
						player->setPreviousState(player->getCurrentState());
						player->setCurrentState(wStrState);
					}
					if (pd == ENUM_PLAYER_DIRECTION_RIGHT)
					{
						wstring wStrState(MG_PLAYER_ANIMATION_STATE_IDLE_RIGHT.begin(), MG_PLAYER_ANIMATION_STATE_IDLE_RIGHT.end());
						player->setPreviousState(player->getCurrentState());
						player->setCurrentState(wStrState);
					}
				}
			}

			player->updateSprite();
		}
		
	}
	// NOW UPDATE THE REST OF THE SPRITES ANIMATION FRAMES/STATES/ROTATIONS
	list<Bot*>::iterator botIterator = bots.begin();

	list<Bot*> markedBots;
	while (botIterator != bots.end())
	{
		Bot *bot = (*botIterator);
		PhysicalProperties *pp = bot->getPhysicalProperties();

		/*PATHFINDING*/
		GameStateManager *gsm = game->getGSM();

		if (gsm->getPath() == NULL) {
			OrthographicGridPathfinder *path = new OrthographicGridPathfinder(Game::getSingleton());
			gsm->setPath(path);
		}

			if (bot->getCurrentPathToFollow()->empty()) {
				gsm->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());
			}

			else {
				gsm->getPath()->updatePath(bot);
			}

			bot->getBoundingVolume()->setCenterX(pp->getX() + (bot->getSpriteType()->getTextureWidth() / 2));
			bot->getBoundingVolume()->setCenterY(pp->getY() + (bot->getSpriteType()->getTextureHeight() / 2));
			bot->getPhysicalProperties()->update();

		bot->think();
		bot->updateSprite();
		if (bot->isMarkedForRemoval())
			markedBots.push_back(bot);
		botIterator++;
	}

	// NOW REMOVE ALL THE MARKED BOTS FROM THE SPRITE MANAGER
	list<Bot*>::iterator markedBotsIterator = markedBots.begin();
	while (markedBotsIterator != markedBots.end())
	{
		Bot *bot = (*markedBotsIterator);
		bots.remove(bot);
		markedBotsIterator++;
	}

	// AND UPDATE THE SPAWNING POOLS
	vector<BotSpawningPool*>::iterator spawningPoolIterator = spawningPools.begin();
	while (spawningPoolIterator != spawningPools.end())
	{
		BotSpawningPool *pool = (*spawningPoolIterator);
		pool->update();

		// AND ONTO THE NEXT SPAWNING POOL
		spawningPoolIterator++;
	}
}

void SpriteManager::addLevelObject(LevelObjectSprite * losToAdd)
{
	losList.push_back(losToAdd);
}

LevelObjectSprite* SpriteManager::removeLevelObject(LevelObjectSprite *losToRemove)
{
	this->losList.remove(losToRemove);
	return losToRemove;
}

void SpriteManager::fireBullet(AnimatedSprite *sprite, bool isplayer, bool safety)
{
	BulletRecycler *bulletRecycler = this->getBulletRecycler();
	LevelObjectSprite *bullet = bulletRecycler->retrieveBullet(L"bullet");
	bullet->setplayer(isplayer);

	addLevelObject(bullet);

	bullet->setSafetyon(safety);

	PhysicalProperties *pp = bullet->getPhysicalProperties();
	PhysicalProperties *spritepp = sprite->getPhysicalProperties();

	if ((!(player->isStrafing()) && player->getPlayerDirection() == ENUM_PLAYER_DIRECTION_DOWN) ||
		(player->isStrafing() &&
		(player->getPreviousState().compare(L"IDLE_BACK") == 0
		|| player->getPreviousState().compare(L"PUNCH_BACK") == 0
		|| player->getPreviousState().compare(L"WALK_BACK") == 0
		|| player->getPreviousState().compare(L"SHOOT_BACK") == 0
		|| player->getPreviousState().compare(L"DAMAGE_BACK") == 0
		|| player->getPreviousState().compare(L"STRAFE_BACK") == 0)))
	{
		bullet->setRotationInRadians(M_PI / 2);
		pp->setX(spritepp->getX() + 20);
		pp->setY(spritepp->getY() + 64);
		pp->setVelocity(0, 30);
	}
	else if ((!(player->isStrafing()) && player->getPlayerDirection() == ENUM_PLAYER_DIRECTION_UP) ||
		(player->isStrafing() &&
		(player->getPreviousState().compare(L"IDLE_FRONT") == 0
		|| player->getPreviousState().compare(L"PUNCH_FRONT") == 0
		|| player->getPreviousState().compare(L"WALK_FRONT") == 0
		|| player->getPreviousState().compare(L"SHOOT_FRONT") == 0
		|| player->getPreviousState().compare(L"DAMAGE_FRONT") == 0
		|| player->getPreviousState().compare(L"STRAFE_FRONT") == 0)))
	{
		bullet->setRotationInRadians(M_PI / 2);
		pp->setX(spritepp->getX() + 30);
		pp->setY(spritepp->getY());
		pp->setVelocity(0, -30);
	}
	else if ((!(player->isStrafing()) && player->getPlayerDirection() == ENUM_PLAYER_DIRECTION_LEFT) ||
		(player->isStrafing() &&
		(player->getPreviousState().compare(L"IDLE_LEFT") == 0
		|| player->getPreviousState().compare(L"PUNCH_LEFT") == 0
		|| player->getPreviousState().compare(L"WALK_LEFT") == 0
		|| player->getPreviousState().compare(L"SHOOT_LEFT") == 0
		|| player->getPreviousState().compare(L"DAMAGE_LEFT") == 0
		|| player->getPreviousState().compare(L"STRAFE_LEFT") == 0)))
	{
		pp->setX(spritepp->getX());
		pp->setY(spritepp->getY() + 40);
		pp->setVelocity(-30, 0);
	}
	else if ((!(player->isStrafing()) && player->getPlayerDirection() == ENUM_PLAYER_DIRECTION_RIGHT) ||
		(player->isStrafing() &&
		(player->getPreviousState().compare(L"IDLE_RIGHT") == 0
		|| player->getPreviousState().compare(L"PUNCH_RIGHT") == 0
		|| player->getPreviousState().compare(L"WALK_RIGHT") == 0
		|| player->getPreviousState().compare(L"SHOOT_RIGHT") == 0
		|| player->getPreviousState().compare(L"DAMAGE_RIGHT") == 0
		|| player->getPreviousState().compare(L"STRAFE_RIGHT") == 0)))
	{
		pp->setX(spritepp->getX()+ 60);
		pp->setY(spritepp->getY() + 40);
		pp->setVelocity(30, 0);
	}
}