/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameText.cpp

	See GameText.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\resources\EngineConfigProperties.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\importers\MGEngineConfigImporter.h"
#include "mg\text\GameText.h"

/*
	GameText - Default constructor, it initializes the data structure for 
	storing all text and sets the default text color to white.
*/
GameText::GameText()
{

}

/*
	~GameText - Destructor, it cleans up the textToDraw vector and the
	textGenerator.
*/
GameText::~GameText()
{

}

void GameText::startUp()
{
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();
	MGEngineConfigImporter *importer = (MGEngineConfigImporter*)resources->getEngineConfigImporter();
	EngineConfigProperties props;
	initDebugFile(importer->getProperty(props.PROP_NAME_DEBUG_FILE));
}

void GameText::shutDown()
{

}

/*
	addRenderText - This method adds text for rendering. Text only needs to
	be added once.
*/
void GameText::addRenderText(wstring *textToAdd,
					   int initX,
					   int initY,
					   int initWidth,
					   int initHeight)
{
	RenderText *text = new RenderText();
	text->setText(textToAdd);
	text->x = initX;
	text->y = initY;
	text->width = initWidth;
	text->height = initHeight;
	renderText.push_back(text);
}

/*
	changeTextOnly - Once text is added, if we want to subsequently
	change it, we may use this method.
*/
void GameText::updateRenderText(wstring *textToSet, int index)
{
	RenderText *textToUpdate = renderText.at(index);
	wstring *oldText = textToUpdate->getText();
	delete oldText;
	textToUpdate->setText(textToSet);
}

/*
	initDebugFile - This method sets up the writer to allow us to write
	to the debug output text file.
*/
void GameText::initDebugFile(wstring debugFileName)
{
	debugWriter.initFile(debugFileName);
}

/*
	moveText - This method allows us to move the location
	of where we draw our text.
*/
void GameText::moveRenderText(int index, int xMove, int yMove)
{
	int x = renderText.at(index)->x;
	int y = renderText.at(index)->y;
	renderText.at(index)->x = x + xMove;
	renderText.at(index)->y = y + yMove;
}

/*
	writeOutput - This method writes text to the debugging log file.
*/
void GameText::writeDebugOutput(wstring output)	
{	
	debugWriter.writeText(output);
}