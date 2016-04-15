#include "mg_VS\stdafx.h"
#include "mg\gsm\sprite\LevelObjectSprite.h"


LevelObjectSprite::LevelObjectSprite()
{
}


LevelObjectSprite::~LevelObjectSprite()
{
}

LevelObjectSprite* LevelObjectSprite::clone()
{
	LevelObjectSprite *clonedBot = new LevelObjectSprite();
	clonedBot->alpha = alpha;
	clonedBot->animationCounter = animationCounter;
	//clonedBot->botState = botState;
	clonedBot->isplayer = isplayer;
	clonedBot->currentlyCollidable = currentlyCollidable;
	clonedBot->currentState = currentState;
	clonedBot->frameIndex = frameIndex;
	clonedBot->rotationInRadians = rotationInRadians;
	clonedBot->spriteType = spriteType;
	clonedBot->type = type;
	return clonedBot;
}