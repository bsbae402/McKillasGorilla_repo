#pragma once

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\sprite\AnimatedSprite.h"

//// these two sprite states values will be common to the all level objects (xml making regulation)
static const string BEING_LEVEL_OBJECT_SPRITE_STATE_VALUE = "BEING";
static const string VANISHING_LEVEL_OBJECT_SPRITE_STATE_VALUE = "VANISHING";

class LevelObjectSprite : public AnimatedSprite
{
private:
	wstring type;
	bool isplayer;
	
public:
	LevelObjectSprite();
	~LevelObjectSprite();

	wstring getType() { return type; };

	void setType(wstring initType)
	{
		type = initType;
	}
	void setplayer(bool newisplayer)
	{
		isplayer = newisplayer;
	}
	bool getPlayer() {
		return isplayer;
	}


	LevelObjectSprite* clone();
};

