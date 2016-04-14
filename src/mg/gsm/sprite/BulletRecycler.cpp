#include "mg_VS\stdafx.h"
#include "mg\gsm\sprite\LevelObjectSprite.h"
#include "mg\gsm\sprite\BulletRecycler.h"
#include "mg\gsm\sprite\SpriteManager.h"


BulletRecycler::BulletRecycler()
{
	
}

/*
A BotRecycler is constructed only once while a game app is
running, at startup. When it is deconstructed, it must delete
all remaining bots.
*/
BulletRecycler::~BulletRecycler()
{
	// GO THROUGH ALL THE TYPES OF BOTS AND DELETE ALL OF THEM
}

/*
It is assumed that a game will have many different types of Bots, thinking
in different ways, some of them even custom bots defined by the game app rather
than by this framework. So, in order to make these bots recyclable, we have to
register them with this recycler so that this recycler knows how to make new
ones as needed.
*/
void BulletRecycler::registerItemType(wstring itemType, LevelObjectSprite *firstBullet)
{
	// FIRST MAKE SURE WE DON'T ALREADY HAVE A BUNCH OF RECYCLABLE
	// BOTS FOR THIS TYPE. TO DO THIS, WE USE SOME C++ WEIRDNESS.
	if (!isRegisteredItemType(itemType) && itemType.compare(L"bullet") == 0)
	{
		// REGISTER THE BOT
		registeredItemTypes[itemType] = firstBullet;
		recyclableItems[itemType] = new list<LevelObjectSprite*>();
		addMoreBullets(itemType, 1);
	}
}

bool BulletRecycler::isRegisteredItemType(wstring itemType)
{
	// FIRST MAKE SURE THIS IS A REGISTERED BOT TYPE,
	// IF IT IS NOT, WE NEED TO RETURN NULL
	map<wstring, LevelObjectSprite *>::iterator testIt = registeredItemTypes.find(itemType);
	if (testIt == registeredItemTypes.end())
		return false;
	else
		return true;
}

LevelObjectSprite* BulletRecycler::retrieveBullet(wstring itemType)
{
	// FIRST MAKE SURE THIS IS A REGISTERED BOT TYPE,
	// IF IT IS NOT, WE NEED TO RETURN NULL
	if (!isRegisteredItemType(itemType))
	{
		return nullptr;
	}
	else
	{
		// GET THE CORRECT LIST OF BOT TYPES
		list<LevelObjectSprite *> *itemsOfTypeWeNeed = recyclableItems[itemType];

		// MAKE SURE WE ARE NOT OUT OF THIS TYPE OF BOT
		if (itemsOfTypeWeNeed->size() == 0)
			addMoreBullets(itemType, 1);

		// NOW GET THE LAST ELEMENT
		LevelObjectSprite* itemToReturn = itemsOfTypeWeNeed->back();
		itemsOfTypeWeNeed->pop_back();
		return itemToReturn;
	}
}

void BulletRecycler::recycleBullet(LevelObjectSprite* itemToRecycle)
{
	wstring itemType = itemToRecycle->getType();
	if (isRegisteredItemType(itemType))
	{
		list<LevelObjectSprite *> *recycleItems = recyclableItems[itemType];
		recycleItems->push_back(itemToRecycle);
	}
}

void BulletRecycler::addMoreBullets(wstring itemType, unsigned int numToAdd)
{
	Game *game = Game::getSingleton();
	// NOW MAKE THE BOTS
	LevelObjectSprite* sampleItem = registeredItemTypes[itemType];

	for (unsigned int i = 0; i < numToAdd; i++)
	{
		LevelObjectSprite *itemClone = sampleItem->clone();
		//itemClone->setType(L"bullet");
		itemClone->setAlpha(255);
		itemClone->setCurrentlyCollidable(true);
		//itemClone->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType(L"bullet"));
		itemClone->setCurrentState(L"BEING");
		
		recyclableItems[itemType]->push_back(itemClone);
	}
}

void BulletRecycler::unload()
{
	// DELETE ALL THE RECYCLABLE BOTS
	map<wstring, list<LevelObjectSprite *> *>::iterator itemsListIt = recyclableItems.begin();
	while (itemsListIt != recyclableItems.end())
	{
		map<wstring, list<LevelObjectSprite *> *>::iterator tempIt = itemsListIt;
		itemsListIt++;
		list<LevelObjectSprite*> *itemsListToDelete = tempIt->second;
		list<LevelObjectSprite*>::iterator itemIt = itemsListToDelete->begin();
		while (itemIt != itemsListToDelete->end())
		{
			list<LevelObjectSprite*>::iterator tempItemIt = itemIt;
			itemIt++;
			LevelObjectSprite *itemToDelete = (*tempItemIt);
			delete itemToDelete;
		}
		itemsListToDelete->clear();
		delete itemsListToDelete;
	}
	recyclableItems.clear();

	// AND DELETE THE REGISTERED BOTS
	map<wstring, LevelObjectSprite *>::iterator registeredItemTypesIt = registeredItemTypes.begin();
	while (registeredItemTypesIt != registeredItemTypes.end())
	{
		map<wstring, LevelObjectSprite *>::iterator tempIt = registeredItemTypesIt;
		registeredItemTypesIt++;
		LevelObjectSprite *itemToDelete = tempIt->second;
		delete itemToDelete;
	}
	registeredItemTypes.clear();
}