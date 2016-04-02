/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	TextFileReader.cpp

	See TextFileReader.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\text\TextFileReader.h"

/*
	Default Constructor - We don't know the name of the file
	to use yet, so we'll initialize everything to NULL.
*/
TextFileReader::TextFileReader()
{
	moreTextToRead = false;
}

/*
	Deconstructor - Make sure the file stuff doesn't cause
	a memory leak.
*/
TextFileReader::~TextFileReader()
{

}

/*
	closeReader - This method closes the stream used for 
	reading from a file and then deletes the inFile stream. If
	you want to read again from the stream, you must call initFile
	first, which will re-initialize everything.
*/
void TextFileReader::close()
{
	inFile.close();
	moreTextToRead = false;
}

/*
	initFile - This method initializes the stream for reading
	from a file using the file name provided as an argument. After
	calling this method, we are ready to read text.
*/
void TextFileReader::loadFile(wstring initFileName)
{
	fileName = initFileName;
	inFile.clear();
	inFile.open(fileName);
	loadBuffer();
	moreTextToRead = true;
}

/*
	loadBuffer - This method reads all the text in the fileName file and
	loads it into the buffer, one line at a time.
*/
void TextFileReader::loadBuffer()
{
	streamsize size = inFile.rdbuf()->in_avail();
	wstring inputLine;
	while(!inFile.eof())
	{
		getline(inFile, inputLine);
		buffer.push_back(inputLine);
	}
	lineCounter = 0;
}

/*
	readLineOfText - This method reads a line of text from the
	file and returns it. Note that before calling this method, first
	call initFile to setup the stream. Note that this method is
	allocating memory on the heap for 256 characters for each line,
	so that should not be exceeded. Also, remember to delete the
	line when done using it to avoid memory leaks.
*/
wstring TextFileReader::getNextLine()
{
    if (moreTextToRead)
	{
		wstring lineOfText = buffer[lineCounter];
		lineCounter++;
		if (lineCounter == buffer.size())
			moreTextToRead = false;
		return lineOfText;
	}
	else
        return NULL;
}