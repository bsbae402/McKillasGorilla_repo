/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	This class provides a framework for technology-specific
	classes that will manage the rendering of all game textures and text.
	It provides methods for the initialization of objects necessary
	for rendering images and text.

	Rendering should be done by generating a list of RenderItem
	objects each frame and putting them into a list. These
	objects store information about the textures that have
	to be drawn to the screen.
*/

#pragma once

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\game\WStringTable.h"
#include "mg\graphics\RenderList.h"
#include "mg\graphics\TextureManager.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gui\GameGUI.h"
#include "mg\input\GameInput.h"
#include "mg\text\GameText.h"
#include "mg\text\TextFileWriter.h"

const static unsigned int DEFAULT_INIT_GUI_RENDER_LIST_SIZE = 100;
const static unsigned int DEFAULT_INIT_LEVEL_RENDER_LIST_SIZE = 3000;

class TextureManager;

class GameGraphics
{
protected:
	// WE'LL SORE THE SCREEN DIMENSIONS HERE
	int screenWidth;
	int screenHeight;

	// EACH FRAME, RENDER LISTS ARE CREATED, FILLED WITH
	// DATA, THEN USED FOR RENDERING THE FRAME, AND DESTROYED
	RenderList				*guiRenderList;
	RenderList				*worldRenderList;

	// NOTE THAT IF WE REALLY WANTED TO BE SLICK, WE COULD PROVIDE
	// MULTIPLE WORLD RENDER LISTS, ONE FOR EACH Z-LEVEL

	// THE TextureManagers STORE ALL THE IMAGES FOR OUR GAME

	TextureManager			*guiTextureManager;
	TextureManager			*worldTextureManager;

	// TOGGLES WHETHER OR NOT TO RENDER THE TEXT, WHICH IS USEFUL
	// FOR DISPLAYING DEBUG INFORMATION
	bool debugTextShouldBeRendered;

public:	
	// INLINED ACCESSOR METHODS
	RenderList*				getGUIRenderList()			{ return guiRenderList;			}
	TextureManager*			getGUITextureManager()		{ return guiTextureManager;		}
	RenderList*				getWorldRenderList()		{ return worldRenderList;		}
	TextureManager*			getWorldTextureManager()	{ return worldTextureManager;	}

	// AND A METHOD FOR TURNING DEBUG TEXT ON AND OFF
	void toggleDebugTextShouldBeRendered()
	{ debugTextShouldBeRendered = !debugTextShouldBeRendered; }

	// VIRTUAL METHODS TO BE OVERRIDDEN BY TECHNOLOGY-SPECIFIC CLASSES
	virtual	TextureManager* createTextureManager()=0;
	virtual int				getScreenHeight()=0;
	virtual int				getScreenWidth()=0;
	virtual void			reloadGraphics()=0;
	virtual void			renderGame()=0;
	virtual void			renderText(RenderText *renderText)=0;
	virtual void			setColorKey(int r, int g, int b)=0;
	virtual void			setFontColor(int r, int g, int b)=0;
	virtual void			setDebugFont(int fontSize)=0;
	virtual void			startUpPlatform()=0;
	virtual void			shutDownPlatform()=0;

	// CONCRETE METHODS DEFINED IN GameGraphics.cpp
	GameGraphics();
	virtual ~GameGraphics();
	void clearWorldTextures();
	void fillRenderLists();
	void initColors();
	void initDebugFont();
	void renderAllText();
	void startUp();
	void shutDown();
};