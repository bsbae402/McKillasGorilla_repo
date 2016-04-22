/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	SpriteManager.h

	This class manages all of the sprites in a given game level. Note
	that the player sprite is also managed by this class.
*/

#pragma once
#include "mg_VS\stdafx.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\gsm\ai\Bot.h"
#include "mg\gsm\ai\BotRecycler.h"
#include "mg\gsm\ai\BotSpawningPool.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\sprite\AnimatedSpriteType.h"

#include "mg\gsm\sprite\LevelObjectSprite.h"
#include "mg\gsm\sprite\PlayerSprite.h"
#include "mg\gsm\sprite\BulletRecycler.h"

class SpriteManager
{
private:
	// NOTE THAT MULTIPLE SPRITES MAY SHARE ARTWORK, SO SPRITE TYPES
	// SPECIFIES A TYPE OF SPRITE, OF WHICH THERE MAY BE MANY INSTANCES
	vector<AnimatedSpriteType*> spriteTypes;
	map<wstring, AnimatedSpriteType*> spriteTypesByName;

	// THESE ARE THE BOTS IN THE GAME, LIKE ENEMIES, ROCKETS, OR ANYTHING
	// THAT MOVES AROUND AND IS NOT THE PLAYER
	list<Bot*> bots;

	list<LevelObjectSprite*> bullets;

	// AND THIS IS THE PLAYER. AS-IS, WE ONLY ALLOW FOR ONE PLAYER AT A TIME
	//// AnimatedSprite *player;
	PlayerSprite *player;

	// THE BotRecycler MAKES SURE WE DON'T HAVE TO CONSTRUCT BOTS WHENEVER
	// WE NEED TO SPAWN THEM, INSTEAD IT WILL RECYCLE THEM FOR US
	BotRecycler botRecycler;

	BulletRecycler bulletRecycler;

	// THESE ARE THIS LEVEL'S SPAWNING POOLS
	vector<BotSpawningPool*> spawningPools;

	//// ---- our new sprite category: level object (los == level object sprite) ---
	list<LevelObjectSprite*> losList;

	int facing = 0;

public:
	// NOTHING TO INIT OR DESTROY
	SpriteManager() { player = nullptr; }
	~SpriteManager()	{}

	// INLINED ACCESSOR METHODS
	BotRecycler*			getBotRecycler()		{ return &botRecycler;		}
	BulletRecycler*			getBulletRecycler() { return &bulletRecycler; }
	int						getNumberOfBots()		{ return bots.size();		}
	PlayerSprite*			getPlayer()				{ return player;			}
	list<Bot*>::iterator	getBotsIterator()		{ return bots.begin();		}
	list<Bot*>::iterator	getEndOfBotsIterator()	{ return bots.end();		}
	vector<BotSpawningPool*>*  getSpawningPools()		{ return &spawningPools;		}

	// INLINED METHOD FOR ADDING A SPAWNING POOL
	void addSpawningPool(BotSpawningPool *poolToAdd)
	{
		spawningPools.push_back(poolToAdd);
	}
	void setPlayer(PlayerSprite *initPlayer)
	{
		player = initPlayer;
	}

	void setFacing(int newfacing) { facing = newfacing; }
	int getFacing() { return facing; }

	// METHODS DEFINED IN SpriteManager.cpp
	void				addBot(Bot *botToAdd);
	void				addSpriteItemsToRenderList();
	unsigned int		addSpriteType(wstring spriteTypeName, AnimatedSpriteType *spriteTypeToAdd);
	void				addSpriteToRenderList(AnimatedSprite *sprite, RenderList *renderList, Viewport *viewport);
	AnimatedSpriteType* getSpriteType(unsigned int typeIndex);
	AnimatedSpriteType* getSpriteType(wstring spriteTypeName);
	Bot*				removeBot(Bot *botToRemove);
	void				unloadSprites();
	void				update();

	void				fireBullet(AnimatedSprite *sprite, bool isplayer, bool safety);

	//// ---- methods for level object management ----
	list<LevelObjectSprite*>::iterator	getLevelSpriteObjectsIterator() { return losList.begin(); }
	list<LevelObjectSprite*>::iterator	getEndOfLevelSpriteObjectsIterator() { return losList.end(); }
	void				addLevelObject(LevelObjectSprite *losToAdd);
	LevelObjectSprite*				removeLevelObject(LevelObjectSprite *losToRemove);
};