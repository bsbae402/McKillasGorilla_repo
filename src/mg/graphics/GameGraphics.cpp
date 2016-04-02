/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameGraphics.cpp

	See GameGraphics.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\game\IllegalArgumentException.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\graphics\TextureManager.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\world\World.h"
#include "mg\gui\GameGUI.h"
#include "mg\os\GameOS.h"
#include "mg\resources\EngineConfigProperties.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGEngineConfigImporter.h"
#include "mg\text\GameText.h"
#include "mg\text\TextFileWriter.h"

/*
	GameGraphics - Default constructor, nothing to initialize.
*/
GameGraphics::GameGraphics()
{

}

/*
	~GameGraphics - Destructor, it cleans up the render lists and texture
	managers. This should only be called when the application is closing.
*/
GameGraphics::~GameGraphics()
{

}

/*
	clearWorldTextures - When the game leaves a level we have to clear
	out these data structures. Calling clear on these will delete
	all the objects inside.
*/
void GameGraphics::clearWorldTextures()
{	
	// CLEAR LEVEL DATA STRUCURES
	worldTextureManager->clear();
	worldRenderList->clear();
}

/*
	fillRenderLists - This method causes the render lists to be 
	filled with the things that have to be drawn this frame.
*/
void GameGraphics::fillRenderLists()
{
	// GET THE GAME SINGLETON
	Game *game = Game::getSingleton();

	// GENERATE RENDER LISTS FOR GAME WORLD AND GUI
	GameStateManager *gsm = game->getGSM();
	gsm->addGameRenderItemsToRenderList();
	GameGUI *gui = game->getGUI();
	gui->addRenderItemsToRenderList();
}

/*
	init - This method constructs the data structures for managing textures
	and render lists. It calls the createTextureManager, which is technology
	specific, and so is implemented only by child classes.
*/
void GameGraphics::startUp()
{
	// FIRST INITIALIZE ALL PLATFORM INDEPENDENT GRAPHICS STUFF
	debugTextShouldBeRendered = false;

	// INIT SCREEN DIMENSIONS
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();
	MGEngineConfigImporter *importer = (MGEngineConfigImporter*)resources->getEngineConfigImporter();
	void* properties = importer->getPropertyTypes();
	EngineConfigProperties props;
	wstring screenWidthProp = importer->getProperty(props.PROP_NAME_SCREEN_WIDTH);
	wstring screenHeightProp = importer->getProperty(props.PROP_NAME_SCREEN_HEIGHT);
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;

	// GUI TEXTURES (like buttons, cursor, etc.)
	guiRenderList = new RenderList(DEFAULT_INIT_GUI_RENDER_LIST_SIZE);
	guiTextureManager = createTextureManager();

	// LEVEL TEXTURES (like sprites, tiles, particles, etc.)
	worldRenderList = new RenderList(DEFAULT_INIT_LEVEL_RENDER_LIST_SIZE);
	worldTextureManager = createTextureManager();

	// AND THEN THE PLATFORM-DEPENDENT STUFF
	startUpPlatform();

	// INITIALIZE COLORS AND FONTS, ALSO 
	initColors();
	initDebugFont();
}

void GameGraphics::shutDown()
{
	delete guiRenderList;
	delete guiTextureManager;
	delete worldRenderList;
	delete worldTextureManager;
}

/*
	renderText - This method will go through the GameText argument,
	pull out each TextToDraw object, and use a technology-specific
	method in a child class, renderTextToDraw, to render each
	piece of text.
*/
void GameGraphics::renderAllText()
{
	if (debugTextShouldBeRendered)
	{
		GameText *text = Game::getSingleton()->getText();
		int numTextObjects = text->getRenderTextSize();
		for (int i = 0; i < numTextObjects; i++)
		{
			RenderText *textToDraw = text->getRenderTextAtIndex(i);
			renderText(textToDraw);
		}
	}
}

/*
	initColors - this helper method loads the color key, used for loading
	images, and the font color, used for rendering text.
*/
void GameGraphics::initColors()
{
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();
	MGEngineConfigImporter *importer = (MGEngineConfigImporter*)resources->getEngineConfigImporter();
	void* properties = importer->getPropertyTypes();
	EngineConfigProperties props;
	int fontRed, fontGreen, fontBlue;
	wstring fontRedProp = importer->getProperty(props.PROP_NAME_FONT_COLOR_RED);
	wstring fontGreenProp = importer->getProperty(props.PROP_NAME_FONT_COLOR_GREEN);
	wstring fontBlueProp = importer->getProperty(props.PROP_NAME_FONT_COLOR_BLUE);
	wstringstream(fontRedProp) >> fontRed;
	wstringstream(fontGreenProp) >> fontGreen;
	wstringstream(fontBlueProp) >> fontBlue;

	// COLOR USED FOR RENDERING TEXT
	setFontColor(fontRed, fontGreen, fontBlue);

	int keyRed, keyGreen, keyBlue;
	wstring keyRedProp = importer->getProperty(props.PROP_NAME_COLOR_KEY_RED);
	wstring keyGreenProp = importer->getProperty(props.PROP_NAME_COLOR_KEY_GREEN);
	wstring keyBlueProp = importer->getProperty(props.PROP_NAME_COLOR_KEY_BLUE);
	wstringstream(keyRedProp) >> keyRed;
	wstringstream(keyGreenProp) >> keyGreen;
	wstringstream(keyBlueProp) >> keyBlue;

	// COLOR KEY - COLOR TO BE IGNORED WHEN LOADING AN IMAGE
	// NOTE, IF YOU WISH TO USE PNG IMAGES THAT CONTAIN ALPHA
	// CHANNEL DATA, YOU DON'T NEED A COLOR KEY
	setColorKey(keyRed, keyGreen, keyBlue);
}

/*
	initFont - this helper method loads the font used for rendering text.
*/
void GameGraphics::initDebugFont()
{
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();
	MGEngineConfigImporter *importer = (MGEngineConfigImporter*)resources->getEngineConfigImporter();
	void* properties = importer->getPropertyTypes();
	EngineConfigProperties props;
	int debugFontSize;
	wstring fontSizeProp = importer->getProperty(props.PROP_NAME_DEBUG_TEXT_FONT_SIZE);
	wstringstream(fontSizeProp) >> debugFontSize;

	// FONT USED FOR RENDERING DEBUG TEXT
	setDebugFont(debugFontSize);
}