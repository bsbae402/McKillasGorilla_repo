/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	WindowsInput.cpp

	See WindowsInput.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\gui\Button.h"
#include "mg\gui\Cursor.h"
#include "mg\game\Game.h"
#include "mg\gsm\state\GameState.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gui\GameGUI.h"
#include "mg\gui\ScreenGUI.h"
#include "mg\input\GameInput.h"
#include "mg\input\KeyEventHandler.h"
#include "mg\os\GameOS.h"
#include "mg\platforms\Windows\WindowsInput.h"
#include "mg\platforms\Windows\WindowsOS.h"

/*
	WindowsInput - Default constructor, it will initialize the input state variables.
*/
WindowsInput::WindowsInput()
{

}

/*
	~WindowsInput - Destructor, it will destroy the mousePoint pointer.
*/
WindowsInput::~WindowsInput()
{

}

/*
	processInput - This method updates first gets all input from Windows
	for the mouse and the keyboard. It then asks the event handlers
	to respond to the input.
*/
void WindowsInput::processInput()
{
	Game *game = Game::getSingleton();
	WindowsOS *os = (WindowsOS*)game->getOS();
	WINDOWINFO wi = os->getWindowInfo();
	updateCursorPosition(wi, game->getGUI()->getCursor());
	updateInputState();
	respondToKeyboardInput();
	respondToMouseInput();
}

/*
	respondToMouseInput - This method sends the updated cursor position
	to the GameGUI so that it can update the Button and Cursor states.
	It then checks to see if the left mouse button is pressed, and if
	so, it asks the gui to check to see if it needs to fire an event.
	This should be called once per frame, after input is retrieved.
*/
void WindowsInput::respondToMouseInput()
{
	Game *game = Game::getSingleton();
	GameGUI *gui = game->getGUI();
	GameStateManager *gsm = game->getGSM();
	Viewport *viewport = gui->getViewport();
	gui->updateGUIState(mousePoint->x, mousePoint->y, gsm->getCurrentGameState());
	
	if ( (GetAsyncKeyState(VK_LBUTTON) & 0X8000)
		&& (inputState[VK_LBUTTON].isFirstPress))
	{
		if ((gsm->isGameInProgress()) && viewport->areScreenCoordinatesInViewport(mousePoint->x, mousePoint->y))
			mouseHandler->handleMousePressEvent(mousePoint->x-viewport->getViewportOffsetX(), mousePoint->y-viewport->getViewportOffsetY());

		gui->checkCurrentScreenForAction();
	}
	if (mouseHasMoved())
	{
		mouseHandler->handleMouseMoveEvent(mousePoint->x, mousePoint->y);
	}
}

/*
	updateCursorPosition - This method asks Windows for the position
	of the cursor in screen coordinates. The cursor position is fixed
	to account for windows borders. The values are recorded for use
	and the cursor is updated.
*/
void WindowsInput::updateCursorPosition(WINDOWINFO wi, Cursor *cursor)
{
	Game *game = Game::getSingleton();
	lastMousePoint->x = mousePoint->x;
	lastMousePoint->y = mousePoint->y;
	GetCursorPos(mousePoint);
	mouseX = mousePoint->x;
	mouseY = mousePoint->y;
	GameGraphics *graphics = game->getGraphics();

	// Fix the mouse location
	//mousePoint->x = mousePoint->x - wi.rcWindow.left - wi.rcClient.left;
	//mousePoint->y = mousePoint->y - wi.rcWindow.top - wi.rcClient.top;
	if (mousePoint->x < 0)
	{
		mousePoint->x = 0;
	}
	if (mousePoint->x >= graphics->getScreenWidth())
	{
		mousePoint->x = graphics->getScreenWidth() - 1;
	}
	if (mousePoint->y < 0)
	{
		mousePoint->y = 0;
	}
	if (mousePoint->y >= graphics->getScreenHeight())
	{
		mousePoint->y = graphics->getScreenHeight() - 1;
	}

	cursor->setX(mousePoint->x);
	cursor->setY(mousePoint->y);
}

/*
	updateInputState - This method checks all keys and updates
	their states. This should be called once per frame.
*/
void WindowsInput::updateInputState()
{
	// RESET isPressed FOR ALL KEYS
	for (int i = 0; i < 256; i++)
		inputState[i].isPressed = false;

	// FILL IN isPressed FOR THOSE PRESSED
	for (int j = 0; j < 256; j++)
	{
		if (GetAsyncKeyState(j) & 0X8000)
			inputState[j].isPressed = true;
	}

	// UPDATE wasHeldDown & isFirstPress
	for (int k = 0; k < 256; k++) 
	{
		if (inputState[k].isPressed) 
		{
			if (inputState[k].wasHeldDown)
				inputState[k].isFirstPress = false;
			else 
			{
				inputState[k].wasHeldDown = true;
				inputState[k].isFirstPress = true;
			}
		}
		else 
		{
			inputState[k].wasHeldDown = false;
			inputState[k].isFirstPress = false;
		}
	}
}

void WindowsInput::startUp()
{
	mousePoint = new POINT();
	lastMousePoint = new POINT();
	mousePoint->x = 0;
	mousePoint->y = 0;
}

/*
	shutdown - we are simply using Windows for getting input, but, if we were using
	another library, like DirectInput for getting game controller data, we may have
	to tie up some loose ends here.
*/
void WindowsInput::shutDown()
{
	delete mousePoint;
	delete lastMousePoint;
}

bool WindowsInput::mouseHasMoved()
{
	return (	(mousePoint->x != lastMousePoint->x)
			||	(mousePoint->y != lastMousePoint->y));
}