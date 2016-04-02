/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameInput.cpp

	See GameInput.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gui\Button.h"
#include "mg\gui\Cursor.h"
#include "mg\gui\GameGUI.h"
#include "mg\gui\ScreenGUI.h"
#include "mg\input\GameInput.h"
#include "mg\input\KeyEventHandler.h"
#include "mg\os\GameOS.h"

/*
	GameInput - Default constructor, it will initialize the input state variables.
*/
GameInput::GameInput()
{
	initInputState();
}

/*
	~GameInput - Destructor, it will destroy the mousePoint pointer.
*/
GameInput::~GameInput()
{
}

/*
	initInputState - This method empties all of the data about
	key presses. This would be called at the start of the application.
*/
void GameInput::initInputState()
{
	// RESET ALL KEYS
	for (int i = 0; i < 256; i++)
	{
		inputState[i].isFirstPress	= false;
		inputState[i].isPressed		= false;
		inputState[i].wasHeldDown	= false;
	}
}
