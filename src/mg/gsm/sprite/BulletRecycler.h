#pragma once
#include "mg_VS\stdafx.h"
#include "mg\gsm\sprite\LevelObjectSprite.h"

//static const unsigned int RECYCLABLE_BOT_INCREMENT = 10;

class BulletRecycler
{
private:
	map<wstring, list<LevelObjectSprite *> *> recyclableItems;
	map<wstring, LevelObjectSprite *> registeredItemTypes;

public:
	BulletRecycler();
	~BulletRecycler();
	map<wstring, list<LevelObjectSprite *> *>* getRecyclableBullets() { return &recyclableItems; }
	map<wstring, LevelObjectSprite *>* getRegisteredItemTypes() { return &registeredItemTypes; }
	void addMoreBullets(wstring objectType, unsigned int numToAdd);
	bool isRegisteredItemType(wstring botType);
	void recycleBullet(LevelObjectSprite* botToRecycle);
	void registerItemType(wstring itemType, LevelObjectSprite *firstBullet);
	LevelObjectSprite* retrieveBullet(wstring itemType);
	void unload();
};