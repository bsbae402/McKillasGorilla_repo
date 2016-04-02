/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	GameText.h

	This class allows for easy rendering of text
	to the game screen.
*/

#pragma once
#include "mg_VS\stdafx.h"
#include "mg\text\TextFileReader.h"
#include "mg\text\TextFileWriter.h"

class Game;
class TextGenerator;

const int MAX_TEXT_OBJECTS = 10;
const static wchar_t DEFAULT_DELIMITER = ':';

class RenderText
{
private:
	wstring *text;

public:
	RenderText()	{}
	~RenderText()	{}
	int maxChars;
	int numChars;
	int x;
	int y;
	int width;
	int height;

	void setText(wstring *textToSet)
	{
		text = textToSet;
	}
	wstring* getText() { return text; }
};

class GameText
{
private:
	// USED FOR RENDERING TEXT
	int textCounter;
	vector<RenderText*> renderText;
	TextGenerator *textGenerator;

	// THIS IS SPECIFICALLY FOR WRITING DEBUG INFO
	TextFileWriter debugWriter;

public:
	// INLINED ACCESSOR METHODS
	int				getRenderTextSize()				{	return renderText.size();		}
	RenderText*		getRenderTextAtIndex(int index) { return renderText.at(index); }
	TextGenerator*	getTextGenerator()				{	return textGenerator;			}

	// INLINED MUTATOR METHOD
	void setTextGenerator(TextGenerator *initTextGenerator)
	{
		textGenerator = initTextGenerator;
	}

	// METHODS DEFINED IN GameText.cpp
	GameText();
	~GameText();

	// INTRO/OUTRO METHODS 
	void startUp();
	void shutDown();

	// METHODS FOR RENDER TEXT
	void addRenderText(wstring *textToAdd, int initX, int initY, int initWidth, int initHeight);
	void updateRenderText(wstring *textToAdd, int index);
	void moveRenderText(int index, int xMove, int yMove);

	// METHODS FOR DEBUG TEXT FILE
	void initDebugFile(wstring debugFileName);
	void writeDebugOutput(wstring output);
};