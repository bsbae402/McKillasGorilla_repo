/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	WindowsInput.h

	This class manages all game input. This is currently done only
	via a keyboard and mouse. Windows method are used to get input
	information each frame.
*/

#pragma once
#include "mg_VS\stdafx.h"
#include "mg\input\KeyEventHandler.h"

#define NUM_MOUSE_BUTTONS 2

// FORWARD DECLARATIONS OF CLASSES USED BY WindowsInput.h
class Cursor;
class Game;
class GameGUI;

class WindowsInput : public GameInput
{
private:
	// THIS IS THE LOCATION OF THE CURSOR
	POINT *mousePoint;
	POINT *lastMousePoint;

public:
	// INLINED METHODS

	// GETS THE MOUSE LOCATION
	POINT*	getMousePoint()						
	{	return mousePoint;								}

	// METHODS DEFINED IN GameInput.cpp
	WindowsInput();
	~WindowsInput();
	void initInputState();
	bool mouseHasMoved();
	void updateInputState();
	void updateCursorPosition(WINDOWINFO wi, Cursor *cursor);
	void processInput();
	void respondToMouseInput();
	void shutDown();
	void startUp();
};