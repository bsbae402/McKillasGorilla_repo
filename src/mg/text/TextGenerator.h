/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	TextGenerator.h

	This class provides a framework for generating text each frame.
	Each game application may decide to generate different text, so
	such a subclass would define initText and updateText to do so.
*/

#pragma once
#include "mg_VS\stdafx.h"

class Game;

class TextGenerator
{
protected:
	TextGenerator() {}
	~TextGenerator(){}
public:
	virtual void startUp() = 0;
	virtual void update()=0;
	virtual void setdebug(wstring newdebug) = 0;
	virtual void setTime(int time) = 0;
	virtual void setHeal(wstring heal) = 0;
};