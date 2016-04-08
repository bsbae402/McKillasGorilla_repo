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
		delete player;

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

	//////**************** TBI: unload level objects *****************///////
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
	// UPDATE THE PLAYER SPRITE ANIMATION FRAME/STATE/ROTATION
	if (player != nullptr)
		player->updateSprite();

	// NOW UPDATE THE REST OF THE SPRITES ANIMATION FRAMES/STATES/ROTATIONS
	list<Bot*>::iterator botIterator = bots.begin();
	list<Bot*> markedBots;
	while (botIterator != bots.end())
	{
		Bot *bot = (*botIterator);
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
