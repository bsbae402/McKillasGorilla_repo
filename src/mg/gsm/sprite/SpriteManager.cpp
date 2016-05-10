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
#include "mg\gsm\ai\pathfinding\OrthographicGridPathfinder.h"

#include "mg\platforms\DirectX\GameAudio.h"

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

				if (!game->getGSM()->getSpriteManager()->getPlayer()->isStrafing()
					&& (game->getGSM()->getSpriteManager()->getPlayer()->getCurrentState().compare(L"DYING") != 0
						&& game->getGSM()->getSpriteManager()->getPlayer()->getCurrentState().compare(L"DEAD") != 0))
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
		if (game->getGSM()->getPhysics()->isActivated() && bot->getInjured() == false)
		{
			/*PATHFINDING*/
			GameStateManager *gsm = game->getGSM();

			if (bot->getFoundPlayer() == false)
			{
				if (bot->getPath() == NULL) {
					OrthographicGridPathfinder *path = new OrthographicGridPathfinder(Game::getSingleton());
					bot->setPath(path);
					bot->setCurrentPathToFollow();
				}

				if (bot->getCurrentPathToFollow()->empty()) {
					//if (bot->getCurrentPathToFollow() == NULL) {
					bot->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());
				}

				else {
					bot->getPath()->updatePath(bot);
				}
			}
			bot->getBoundingVolume()->setCenterX(pp->getX() + (bot->getSpriteType()->getTextureWidth() / 2));
			bot->getBoundingVolume()->setCenterY(pp->getY() + (bot->getSpriteType()->getTextureHeight() / 2));
			bot->getPhysicalProperties()->update();

			if(bot->getCurrentState().compare(L"IDLE_BACK") == 0
				|| bot->getCurrentState().compare(L"WALK_BACK") == 0
				|| bot->getCurrentState().compare(L"SHOOT_BACK") == 0
				|| bot->getCurrentState().compare(L"PUNCH_BACK") == 0
				|| bot->getCurrentState().compare(L"STRAFE_BACK") == 0
				|| bot->getCurrentState().compare(L"DAMAGE_BACK") == 0)
				bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
			else if (bot->getCurrentState().compare(L"IDLE_FRONT") == 0
				|| bot->getCurrentState().compare(L"WALK_FRONT") == 0
				|| bot->getCurrentState().compare(L"SHOOT_FRONT") == 0
				|| bot->getCurrentState().compare(L"PUNCH_FRONT") == 0
				|| bot->getCurrentState().compare(L"STRAFE_FRONT") == 0
				|| bot->getCurrentState().compare(L"DAMAGE_FRONT") == 0)
				bot->setEnemyDirection(ENUM_Enemy_DIRECTION_UP);
			else if (bot->getCurrentState().compare(L"IDLE_LEFT") == 0
				|| bot->getCurrentState().compare(L"WALK_LEFT") == 0
				|| bot->getCurrentState().compare(L"SHOOT_LEFT") == 0
				|| bot->getCurrentState().compare(L"PUNCH_LEFT") == 0
				|| bot->getCurrentState().compare(L"STRAFE_LEFT") == 0
				|| bot->getCurrentState().compare(L"DAMAGE_LEFT") == 0)
				bot->setEnemyDirection(ENUM_Enemy_DIRECTION_LEFT);
			else if (bot->getCurrentState().compare(L"IDLE_RIGHT") == 0
				|| bot->getCurrentState().compare(L"WALK_RIGHT") == 0
				|| bot->getCurrentState().compare(L"SHOOT_RIGHT") == 0
				|| bot->getCurrentState().compare(L"PUNCH_RIGHT") == 0
				|| bot->getCurrentState().compare(L"STRAFE_RIGHT") == 0
				|| bot->getCurrentState().compare(L"DAMAGE_RIGHT") == 0)
				bot->setEnemyDirection(ENUM_Enemy_DIRECTION_RIGHT);

			checkforplayer(bot);
		}

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
	Game *game = Game::getSingleton();
	
	// send shooting sound activate signal to audio engine
	GameAudio *audio = game->getAudio();
	audio->recieveShootSoundSignal();

	BulletRecycler *bulletRecycler = this->getBulletRecycler();
	LevelObjectSprite *bullet = bulletRecycler->retrieveBullet(L"bullet");
	bullet->setplayer(isplayer);

	addLevelObject(bullet);

	bullet->setSafetyon(safety);

	bullet->setAttack(game->getGSM()->getSpriteManager()->getPlayer()->getAttack());
	
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

void SpriteManager::fireEnemyBullet(Bot *sprite, bool isplayer, bool safety)
{
	// send shoot sound signal to audio engine
	Game *game = Game::getSingleton();
	GameAudio *audio = game->getAudio();
	audio->recieveShootSoundSignal();
	// shoot sound signal complete

	BulletRecycler *bulletRecycler = this->getBulletRecycler();
	LevelObjectSprite *bullet = bulletRecycler->retrieveBullet(L"bullet");
	bullet->setplayer(isplayer);

	addLevelObject(bullet);

	bullet->setSafetyon(safety);

	bullet->setAttack(sprite->getAttack());

	PhysicalProperties *pp = bullet->getPhysicalProperties();
	PhysicalProperties *spritepp = sprite->getPhysicalProperties();

	if ((!(sprite->isStrafing()) && sprite->getEnemyDirection() == ENUM_PLAYER_DIRECTION_DOWN) ||
		(sprite->isStrafing() &&
			(sprite->getPreviousState().compare(L"IDLE_BACK") == 0
				|| sprite->getPreviousState().compare(L"PUNCH_BACK") == 0
				|| sprite->getPreviousState().compare(L"WALK_BACK") == 0
				|| sprite->getPreviousState().compare(L"SHOOT_BACK") == 0
				|| sprite->getPreviousState().compare(L"DAMAGE_BACK") == 0
				|| sprite->getPreviousState().compare(L"STRAFE_BACK") == 0)))
	{
		bullet->setRotationInRadians(M_PI / 2);
		pp->setX(spritepp->getX() + 20);
		pp->setY(spritepp->getY() + 64);
		pp->setVelocity(0, 30);
	}
	else if ((!(sprite->isStrafing()) && sprite->getEnemyDirection() == ENUM_PLAYER_DIRECTION_UP) ||
		(sprite->isStrafing() &&
			(sprite->getPreviousState().compare(L"IDLE_FRONT") == 0
				|| sprite->getPreviousState().compare(L"PUNCH_FRONT") == 0
				|| sprite->getPreviousState().compare(L"WALK_FRONT") == 0
				|| sprite->getPreviousState().compare(L"SHOOT_FRONT") == 0
				|| sprite->getPreviousState().compare(L"DAMAGE_FRONT") == 0
				|| sprite->getPreviousState().compare(L"STRAFE_FRONT") == 0)))
	{
		bullet->setRotationInRadians(M_PI / 2);
		pp->setX(spritepp->getX() + 30);
		pp->setY(spritepp->getY());
		pp->setVelocity(0, -30);
	}
	else if ((!(sprite->isStrafing()) && sprite->getEnemyDirection() == ENUM_PLAYER_DIRECTION_LEFT) ||
		(sprite->isStrafing() &&
			(sprite->getPreviousState().compare(L"IDLE_LEFT") == 0
				|| sprite->getPreviousState().compare(L"PUNCH_LEFT") == 0
				|| sprite->getPreviousState().compare(L"WALK_LEFT") == 0
				|| sprite->getPreviousState().compare(L"SHOOT_LEFT") == 0
				|| sprite->getPreviousState().compare(L"DAMAGE_LEFT") == 0
				|| sprite->getPreviousState().compare(L"STRAFE_LEFT") == 0)))
	{
		pp->setX(spritepp->getX());
		pp->setY(spritepp->getY() + 40);
		pp->setVelocity(-30, 0);
	}
	else if ((!(sprite->isStrafing()) && sprite->getEnemyDirection() == ENUM_PLAYER_DIRECTION_RIGHT) ||
		(sprite->isStrafing() &&
			(sprite->getPreviousState().compare(L"IDLE_RIGHT") == 0
				|| sprite->getPreviousState().compare(L"PUNCH_RIGHT") == 0
				|| sprite->getPreviousState().compare(L"WALK_RIGHT") == 0
				|| sprite->getPreviousState().compare(L"SHOOT_RIGHT") == 0
				|| sprite->getPreviousState().compare(L"DAMAGE_RIGHT") == 0
				|| sprite->getPreviousState().compare(L"STRAFE_RIGHT") == 0)))
	{
		pp->setX(spritepp->getX() + 60);
		pp->setY(spritepp->getY() + 40);
		pp->setVelocity(30, 0);
	}
}

void SpriteManager::checkforplayer(Bot *bot)
{
	Game *game = Game::getSingleton();
	PhysicalProperties *pp = bot->getPhysicalProperties();
	PhysicalProperties *playerpp = this->getPlayer()->getPhysicalProperties();

	int vX = pp->getVelocityX();
	int vY = pp->getVelocityY();

	float left = pp->getX();
	float top = pp->getY() + 80;
	float right = pp->getX() + 64;
	float bottom = pp->getY() + 128;

	TiledLayer *collidableLayer = game->getGSM()->getWorld()->getCollidableLayer();

	int leftColumn = collidableLayer->getColumnByX(left);
	int rightColumn = collidableLayer->getColumnByX(right);
	int topRow = collidableLayer->getRowByY(top);
	int bottomRow = collidableLayer->getRowByY(bottom);

	bool dontshoot = false;
	bool wallbetween = false;
	if (playerpp->getY() >= pp->getY() - 150 && playerpp->getY() <= pp->getY() + 150)
	{
		int bottomRowNextFrame = collidableLayer->getRowByY(pp->getY() + 100);

		for (int columnIndex = pp->getX() / 64; columnIndex >= (playerpp->getX() + 64) / 64; columnIndex--) {
			Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);

			if (tile->collidable)
			{

				wallbetween = true;
				break;
			}
		}
	}

	if (wallbetween == false)
	{
		if (playerpp->getY() >= pp->getY() - 150 && playerpp->getY() <= pp->getY() + 150)
		{
			int bottomRowNextFrame = collidableLayer->getRowByY(pp->getY() + 120);

			for (int columnIndex = pp->getX() / 64; columnIndex <= (playerpp->getX() + 64) / 64; columnIndex++) {
				Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
				if (tile->collidable)
				{
					wallbetween = true;
					break;
				}
			}
		}
	}

	if (wallbetween == false)
	{
		if (playerpp->getX() >= pp->getX() - 150 && playerpp->getX() <= pp->getX() + 150)
		{
			int leftColumnNextFrame = collidableLayer->getColumnByX(pp->getX());

			for (int rowIndex = pp->getY() / 64; rowIndex >= (playerpp->getY() + 130) / 64; rowIndex--) {
				Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);

				if (tile->collidable)
				{
					wallbetween = true;
					break;
				}
			}
		}
	}

	if (wallbetween == false)
	{
		if (playerpp->getX() >= pp->getX() - 150 && playerpp->getX() <= pp->getX() + 150)
		{
			int leftColumnNextFrame = collidableLayer->getColumnByX(pp->getX());


			for (int rowIndex = pp->getY() / 64; rowIndex <= (playerpp->getY() + 130) / 64; rowIndex++) {
				Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);

				if (tile->collidable)
				{
					wallbetween = true;
					break;
				}

			}
		}
	}

	if (bot->getEnemyDirection() == ENUM_Enemy_DIRECTION_LEFT)
	{
		if ((wallbetween == false && playerpp->getX() >= pp->getX() - 400 && playerpp->getX() <= pp->getX()
			&& playerpp->getY() >= pp->getY() - 150 && playerpp->getY() <= pp->getY() + 150)
			|| (bot->getFoundPlayer() == true && playerpp->getX() >= pp->getX() - 900 && playerpp->getX() <= pp->getX()
				&& playerpp->getY() >= pp->getY() - 950 && playerpp->getY() <= pp->getY() + 950))
		{
			bot->setFoundPlayer(true);
			if (bot->getPath() != NULL)
				bot->clearPath();


			int bottomRowNextFrame = collidableLayer->getRowByY(pp->getY() + 100);
			int bottomRowNextFrame2 = collidableLayer->getRowByY(pp->getY() + 128);


			for (int columnIndex = pp->getX() / 64; columnIndex >= (playerpp->getX() + 64) / 64; columnIndex--) {
				Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
				Tile *tile2 = collidableLayer->getTile(bottomRowNextFrame2, columnIndex);
				if (tile->collidable)
				{
					int a = pp->getX() / 64;
					int b = (playerpp->getX() + 64)/ 64;
					pp->setVelocity(0, 4);
					bot->setChangeup(playerpp->getX());
					bot->setChangedown(-1);
					dontshoot = true;
					break;
				}
				else if (tile2->collidable)
				{
					pp->setVelocity(0, -4);
					bot->setChangeup(-1);
					bot->setChangedown(playerpp->getX());
					dontshoot = true;
					break;
				}
			}


			if (playerpp->getY() <= pp->getY() + 64 && playerpp->getY() + 128 >= pp->getY() + 64 && dontshoot == false)
			{
				bot->setCurrentState(L"SHOOT_LEFT");
				pp->setVelocity(0, 0);
				if (bot->getFireinterval() <= 0)
				{
					fireEnemyBullet(bot, false, false);
					bot->setFireinterval(30);
					bot->setChangeup(-1);
					bot->setChangedown(-1);
				}
				else
					bot->setFireinterval(bot->getFireinterval() - 1);
			}
			else if (wallbetween == false)
			{
				bot->setCurrentState(L"STRAFE_LEFT");
				float LeftNextFrame = left + vX;

				int test = bot->getChangedown();

				if (pp->getX() <= bot->getChangeup()
					|| (playerpp->getY() + 128 < pp->getY() && playerpp->getX() < pp->getX() + 64
					&& playerpp->getX() + 64 > pp->getX() + 64)
					)
				{
					bot->setCurrentState(L"IDLE_FRONT");
					bot->setChangeup(-1);
					bot->setChangedown(-1);
				}
				else if (pp->getX() <= bot->getChangedown()
					|| (playerpp->getY() > pp->getY() + 128 && playerpp->getX() < pp->getX() + 64
					&& playerpp->getX() + 64 > pp->getX() + 64)
					)
				{
					bot->setCurrentState(L"IDLE_BACK");
					bot->setChangedown(-1);
					bot->setChangeup(-1);
				}
				else if (dontshoot == false && bot->getChangedown() <= 0 && bot->getChangeup() <= 0)
				{
					if (playerpp->getY() <= pp->getY() + 64)
					{
						pp->setVelocity(0, -6);
					}
					else if (playerpp->getY() >= pp->getY())
					{
						pp->setVelocity(0, 6);
					}
				}
				else if (dontshoot == false)
				{
					pp->setVelocity(-8, 0);
				}

				vY = pp->getVelocityY();

				if (vY > 0)
				{
					float BottomNextFrame = bottom + vY;

					if (BottomNextFrame < game->getGSM()->getWorld()->getWorldHeight())
					{
						int bottomRowNextFrame = collidableLayer->getRowByY(BottomNextFrame);

						for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
						{
							Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
							if (tile->collidable)
							{
								//pp->setY(pp->getY() - 6);
								pp->setVelocity(-4, 0);
								bot->setChangeup(-1);
								bot->setChangedown(playerpp->getX());
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
								//pp->setY(pp->getY() + 6);
								pp->setVelocity(-4, 0);
								bot->setChangeup(playerpp->getX());
								bot->setChangedown(-1);
								break;
							}
						}
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
								bot->setFoundPlayer(false);
								break;
							}
						}

					}
				}
				

			}

			//bot->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());


		}
		else
		{
			if (pp->getX() <= bot->getChangeup()
				//|| (playerpp->getY() + 128 < pp->getY() && playerpp->getX() < pp->getX() + 64
					//&& playerpp->getX() + 64 > pp->getX() + 64)
				)
			{
				bot->setCurrentState(L"IDLE_FRONT");
				bot->setChangeup(-1);
				bot->setChangedown(-1);
			}
			else if (pp->getX() <= bot->getChangedown()
				//|| (playerpp->getY() > pp->getY() + 128 && playerpp->getX() < pp->getX() + 64
				//	&& playerpp->getX() + 64 > pp->getX() + 64)
				)
			{
				bot->setCurrentState(L"IDLE_BACK");
				bot->setChangedown(-1);
				bot->setChangeup(-1);
			}
			else
				bot->setFoundPlayer(false);
			//pp->setFinalX(pp->getOriginalFinalX());
			//pp->setFinalY(pp->getOriginalFinalY());
		}
	}
	else if (bot->getEnemyDirection() == ENUM_Enemy_DIRECTION_RIGHT)
	{
		if ((wallbetween == false && playerpp->getX() <= pp->getX() + 400 && playerpp->getX() >= pp->getX()
			&& playerpp->getY() >= pp->getY() - 150 && playerpp->getY() <= pp->getY() + 150)
			|| (bot->getFoundPlayer() == true && playerpp->getX() <= pp->getX() + 900 && playerpp->getX() >= pp->getX()
				&& playerpp->getY() <= pp->getY() - 950 && playerpp->getY() >= pp->getY() + 950))
		{
			bot->setFoundPlayer(true);
			if (bot->getPath() != NULL)
				bot->clearPath();


			int bottomRowNextFrame = collidableLayer->getRowByY(pp->getY() + 120);
			int bottomRowNextFrame2 = collidableLayer->getRowByY(pp->getY() + 128);

			//bool dontshoot = false;

			for (int columnIndex = pp->getX() / 64; columnIndex <= (playerpp->getX() + 64) / 64; columnIndex++) {
				Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
				Tile *tile2 = collidableLayer->getTile(bottomRowNextFrame2, columnIndex);
				if (tile->collidable)
				{
					pp->setVelocity(0, 4);
					bot->setChangeup(playerpp->getX());
					bot->setChangedown(-1);
					dontshoot = true;
					break;
				}
				else if (tile2->collidable)
				{
					pp->setVelocity(0, -4);
					bot->setChangeup(-1);
					bot->setChangedown(playerpp->getX());
					dontshoot = true;
					break;
				}
			}


			if (playerpp->getY() <= pp->getY() + 64 && playerpp->getY() + 128 >= pp->getY() + 64)
			{
				bot->setCurrentState(L"SHOOT_RIGHT");
				pp->setVelocity(0, 0);
				if (bot->getFireinterval() <= 0)
				{
					fireEnemyBullet(bot, false, false);
					bot->setFireinterval(30);
					bot->setChangeup(-1);
					bot->setChangedown(-1);
				}
				else
					bot->setFireinterval(bot->getFireinterval() - 1);
			}
			else
			{
				bot->setCurrentState(L"STRAFE_RIGHT");
				float RightNextFrame = right + vX;

				if (pp->getX() >= bot->getChangeup() && bot->getChangeup() > 0
					|| (playerpp->getY() + 128 < pp->getY() && playerpp->getX() < pp->getX() + 64
					&& playerpp->getX() + 64 > pp->getX() + 64))
				{
					bot->setCurrentState(L"IDLE_FRONT");
					bot->setChangeup(-1);
					bot->setChangedown(-1);
				}
				else if (pp->getX() >= bot->getChangedown() && bot->getChangedown() > 0
					|| (playerpp->getY() > pp->getY() + 128 && playerpp->getX() < pp->getX() + 64
					&& playerpp->getX() + 64 > pp->getX() + 64))
				{
					bot->setCurrentState(L"IDLE_BACK");
					bot->setChangeup(-1);
					bot->setChangedown(-1);
				}
				else if (dontshoot == false && bot->getChangedown() <= 0 && bot->getChangeup() <= 0)
				{
					if (playerpp->getY() + 128 <= pp->getY() + 64)
					{
						pp->setVelocity(0, -8);
					}
					else if (playerpp->getY() >= pp->getY())
					{
						pp->setVelocity(0, 8);
					}
				}
				else if (dontshoot == false)
				{
					pp->setVelocity(8, 0);
				}


				vY = pp->getVelocityY();

				if (vY > 0)
				{
					float BottomNextFrame = bottom + vY;

					if (BottomNextFrame < game->getGSM()->getWorld()->getWorldHeight())
					{
						int bottomRowNextFrame = collidableLayer->getRowByY(BottomNextFrame);

						for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
						{
							Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
							if (tile->collidable)
							{
								//pp->setY(pp->getY() - 6);
								pp->setVelocity(4, 0);
								bot->setChangeup(-1);
								bot->setChangedown(playerpp->getX());
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
								//pp->setY(pp->getY() + 6);
								pp->setVelocity(4, 0);
								bot->setChangeup(playerpp->getX());
								bot->setChangedown(-1);
								break;
							}
						}
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
								bot->setFoundPlayer(false);
								break;
							}
						}

					}
				}



			}

			//bot->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());


		}
		else
		{
			if (pp->getX() >= bot->getChangeup() && bot->getChangeup() > 0
			//	|| playerpp->getY() + 128 < pp->getY() && playerpp->getX() < pp->getX() + 64
			//	&& playerpp->getX() + 64 > pp->getX() + 64
				)
			{
				bot->setCurrentState(L"IDLE_FRONT");
				bot->setChangeup(-1);
				bot->setChangedown(-1);
			}
			else if (pp->getX() >= bot->getChangedown() && bot->getChangedown() > 0
			//	|| playerpp->getY() > pp->getY() + 128 && playerpp->getX() < pp->getX() + 64
			//	&& playerpp->getX() + 64 > pp->getX() + 64
				)
			{
				bot->setCurrentState(L"IDLE_BACK");
				bot->setChangeup(-1);
				bot->setChangedown(-1);
			}
			else
				bot->setFoundPlayer(false);
			//pp->setFinalX(pp->getOriginalFinalX());
			//pp->setFinalY(pp->getOriginalFinalY());
		}
	}
	else if (bot->getEnemyDirection() == ENUM_Enemy_DIRECTION_UP)
	{
		if ((wallbetween == false && playerpp->getY() >= pp->getY() - 400 && playerpp->getY() <= pp->getY()
			&& playerpp->getX() >= pp->getX() - 150 && playerpp->getX() <= pp->getX() + 150)
			|| (bot->getFoundPlayer() == true && playerpp->getY() >= pp->getY() - 900 && playerpp->getY() <= pp->getY()
				&& playerpp->getX() >= pp->getX() - 950 && playerpp->getX() <= pp->getX() + 950))
		{
			bot->setFoundPlayer(true);
			if (bot->getPath() != NULL)
				bot->clearPath();

			int leftColumnNextFrame = collidableLayer->getColumnByX(pp->getX());
			int leftColumnNextFrame2 = collidableLayer->getColumnByX(pp->getX() + 64);

			//bool dontshoot = false;

			for (int rowIndex = pp->getY()/64; rowIndex >= (playerpp->getY() + 130)/64; rowIndex--) {
				Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);
				Tile *tile2 = collidableLayer->getTile(rowIndex, leftColumnNextFrame2);
				if (tile->collidable)
				{
					pp->setVelocity(4, 0);
					bot->setChangeright(-1);
					bot->setChangeleft(playerpp->getY());
					dontshoot = true;
					break;
				}
				else if (tile2->collidable)
				{
					pp->setVelocity(-4, 0);
					bot->setChangeright(playerpp->getY());
					bot->setChangeleft(-1);
					dontshoot = true;
					break;
				}
			}

			if (playerpp->getX() <= pp->getX() + 32 && playerpp->getX() + 64 >= pp->getX() + 32 && dontshoot == false)
			{
				bot->setCurrentState(L"SHOOT_FRONT");
				pp->setVelocity(0, 0);
				if (bot->getFireinterval() <= 0)
				{
					fireEnemyBullet(bot, false, false);
					bot->setFireinterval(30);
					bot->setChangeright(-1);
					bot->setChangeleft(-1);
				}
				else
					bot->setFireinterval(bot->getFireinterval() - 1);
			}
			else 
			{
				bot->setCurrentState(L"STRAFE_FRONT");
				float LeftNextFrame = left + vX;

				if (pp->getY() <= bot->getChangeright() && bot->getChangeright() > 0
					|| (playerpp->getX() > pp->getX() + 64 && playerpp->getY() + 128 > pp->getY() + 64
					&& playerpp->getY() < pp->getY() + 64)
					)
				{
					if (wallbetween == false)
					{
						bot->setCurrentState(L"IDLE_RIGHT");
						bot->setChangeleft(-1);
						bot->setChangeright(-1);
					}
				}
				else if (pp->getY() <= bot->getChangeleft() && bot->getChangeleft() > 0
					|| (playerpp->getX() + 64 < pp->getX() && playerpp->getY() + 128 > pp->getY() + 64
					&& playerpp->getY() < pp->getY() + 64)
					)
				{
					if (wallbetween == false)
					{
						bot->setCurrentState(L"IDLE_LEFT");
						bot->setChangeleft(-1);
						bot->setChangeright(-1);
					}
				}
				//else if (playerpp->getY() > pp->getY())
				//	bot->setEnemyDirection(ENUM_Enemy_DIRECTION_DOWN);
				else if (dontshoot == false && bot->getChangeleft() <= 0 && bot->getChangeright() <= 0)
				{
					if (playerpp->getX() + 64 <= pp->getX() + 32)
					{
						pp->setVelocity(-6, 0);
					}
					else if (playerpp->getX() >= pp->getX() + 32)
					{
						pp->setVelocity(6, 0);
					}
				}
				else if (dontshoot == false)
				{
					pp->setVelocity(0, -8);
				}

				vX = pp->getVelocityX();

				if (vX > 0)
				{
					float RightNextFrame = right + vX;

					if (RightNextFrame > 0.0f) {
						int rightColumnNextFrame = collidableLayer->getColumnByX(RightNextFrame);

						for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
							Tile *tile = collidableLayer->getTile(rowIndex, rightColumnNextFrame);
							if (tile->collidable)
							{
								pp->setVelocity(0, -4);
								bot->setChangeright(playerpp->getY());
								bot->setChangeleft(-1);
								break;
							}
						}
						
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
								pp->setVelocity(0, -4);
								bot->setChangeright(-1);
								bot->setChangeleft(playerpp->getY());
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
								bot->setFoundPlayer(false);
								break;
							}
						}
					}
				}


			}

			//bot->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());


		}
		else if (wallbetween == false)
		{
			if (pp->getY() <= bot->getChangeright() && bot->getChangeright() > 0
			//	|| (playerpp->getX() > pp->getX() + 64 && playerpp->getY() + 128 > pp->getY() + 64
			//	&& playerpp->getY() < pp->getY() + 64)
				)
			{
				bot->setCurrentState(L"IDLE_RIGHT");
				bot->setChangeleft(-1);
				bot->setChangeright(-1);
			}
			else if (pp->getY() <= bot->getChangeleft() && bot->getChangeleft() > 0
			//	|| (playerpp->getX() + 64 < pp->getX() && playerpp->getY() + 128 > pp->getY() + 64
			//	&& playerpp->getY() < pp->getY() + 64)
				)
			{
				bot->setCurrentState(L"IDLE_LEFT");
				bot->setChangeleft(-1);
				bot->setChangeright(-1);
			}
			else
				bot->setFoundPlayer(false);
			//pp->setFinalX(pp->getOriginalFinalX());
			//pp->setFinalY(pp->getOriginalFinalY());
		}
	}
	else if (bot->getEnemyDirection() == ENUM_Enemy_DIRECTION_DOWN)
	{
		if ((wallbetween == false && playerpp->getY() <= pp->getY() + 250 && playerpp->getY() >= pp->getY()
			&& playerpp->getX() >= pp->getX() - 150 && playerpp->getX() <= pp->getX() + 150)
			|| (bot->getFoundPlayer() == true && playerpp->getY() <= pp->getY() + 900 && playerpp->getY() >= pp->getY()
				&& playerpp->getX() >= pp->getX() - 950 && playerpp->getX() <= pp->getX() + 950))
		{
			bot->setFoundPlayer(true);
			if (bot->getPath() != NULL)
				bot->clearPath();

			int leftColumnNextFrame = collidableLayer->getColumnByX(pp->getX());
			int leftColumnNextFrame2 = collidableLayer->getColumnByX(pp->getX() + 64);

			//bool dontshoot = false;

			for (int rowIndex = pp->getY() / 64; rowIndex <= (playerpp->getY() + 130) / 64; rowIndex++) {
				Tile *tile = collidableLayer->getTile(rowIndex, leftColumnNextFrame);
				Tile *tile2 = collidableLayer->getTile(rowIndex, leftColumnNextFrame2);
				if (tile->collidable)
				{
					pp->setVelocity(4, 0);
					bot->setChangeright(-1);
					bot->setChangeleft(playerpp->getY());
					dontshoot = true;
					break;
				}
				else if (tile2->collidable)
				{
					pp->setVelocity(-4, 0);
					bot->setChangeright(playerpp->getY());
					bot->setChangeleft(-1);
					dontshoot = true;
					break;
				}
			}


			if (playerpp->getX() <= pp->getX() + 32 && playerpp->getX() + 64 >= pp->getX() + 32 && dontshoot == false)
			{
				bot->setCurrentState(L"SHOOT_BACK");
				pp->setVelocity(0, 0);
				if (bot->getFireinterval() <= 0)
				{
					fireEnemyBullet(bot, false, false);
					bot->setFireinterval(30);
					bot->setChangeright(-1);
					bot->setChangeleft(-1);
				}
				else
					bot->setFireinterval(bot->getFireinterval() - 1);
			}
			else
			{
				bot->setCurrentState(L"STRAFE_BACK");
				float LeftNextFrame = left + vX;

				if (pp->getY() >= bot->getChangeright() && bot->getChangeright() > 0
					|| (playerpp->getX() > pp->getX() + 64 && playerpp->getY() < pp->getY() + 64
					&& playerpp->getY() > pp->getY()))
				{
					if (wallbetween == false)
					{
						bot->setCurrentState(L"IDLE_RIGHT");
						bot->setChangeleft(-1);
						bot->setChangeright(-1);
					}
				}
				else if (pp->getY() >= bot->getChangeleft() && bot->getChangeleft() > 0
					|| (playerpp->getX() + 64 < pp->getX() && playerpp->getY() < pp->getY() + 64
					&& playerpp->getY() > pp->getY()))
				{
					if (wallbetween == false)
					{
						bot->setCurrentState(L"IDLE_LEFT");
						bot->setChangeleft(-1);
						bot->setChangeright(-1);
					}
				}
				//else if (playerpp->getY() < pp->getY())
					//bot->setEnemyDirection(ENUM_Enemy_DIRECTION_UP);
				else if (dontshoot == false && bot->getChangeleft() <= 0 && bot->getChangeright() <= 0)
				{
					if (playerpp->getX() + 64 <= pp->getX() + 32)
					{
						pp->setVelocity(-6, 0);
					}
					else if (playerpp->getX() >= pp->getX() + 32)
					{
						pp->setVelocity(6, 0);
					}
				}
				else if(dontshoot == false)
				{
					pp->setVelocity(0, 8);
				}


				vX = pp->getVelocityX();

				if (vX > 0)
				{
					float RightNextFrame = right + vX;

					if (RightNextFrame > 0.0f) {
						int rightColumnNextFrame = collidableLayer->getColumnByX(RightNextFrame);

						for (int rowIndex = topRow; rowIndex <= bottomRow; rowIndex++) {
							Tile *tile = collidableLayer->getTile(rowIndex, rightColumnNextFrame);
							if (tile->collidable)
							{
								pp->setVelocity(0, 4);
								bot->setChangeright(playerpp->getY());
								bot->setChangeleft(-1);
								break;
							}
						}

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
								pp->setVelocity(0, 4);
								bot->setChangeright(-1);
								bot->setChangeleft(playerpp->getY());
								break;
							}
						}

					}
				}
				else if (vY > 0)
				{
					float BottomNextFrame = bottom + vY;

					if (BottomNextFrame < game->getGSM()->getWorld()->getWorldHeight())
					{
						int bottomRowNextFrame = collidableLayer->getRowByY(BottomNextFrame);

						for (int columnIndex = leftColumn; columnIndex <= rightColumn; columnIndex++)
						{
							Tile *tile = collidableLayer->getTile(bottomRowNextFrame, columnIndex);
							if (tile->collidable)
							{
								bot->setFoundPlayer(false);
								break;
							}
						}


					}
				}



			}

			//bot->getPath()->mapPath(bot, pp->getFinalX(), pp->getFinalY());


		}
		else if(wallbetween == false)
		{
			if (pp->getY() >= bot->getChangeright() && bot->getChangeright() != -1
			//	|| (playerpp->getX() > pp->getX() + 64 && playerpp->getY() < pp->getY() + 64
			//		&& playerpp->getY() > pp->getY())
				)
			{
				bot->setCurrentState(L"IDLE_RIGHT");
				bot->setChangeleft(-1);
				bot->setChangeright(-1);
			}
			else if (pp->getY() >= bot->getChangeleft() && bot->getChangeleft() != -1
			//	|| (playerpp->getX() + 64 < pp->getX() && playerpp->getY() < pp->getY() + 64
			//		&& playerpp->getY() > pp->getY())
				)
			{
				bot->setCurrentState(L"IDLE_LEFT");
				bot->setChangeleft(-1);
				bot->setChangeright(-1);
			}
			else
				bot->setFoundPlayer(false);
			//pp->setFinalX(pp->getOriginalFinalX());
			//pp->setFinalY(pp->getOriginalFinalY());
		}
	}
}
