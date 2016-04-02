/*
	Author: Richard McKenna
	Stony Brook University
	Computer Science Department

	EngineConfigProperties.h

	This class stores all of the configuration properties for initialization
	of McKilla's Gorilla game engine. These settings are stored in an external
	file and loaded at start up into this singleton. These settings are then
	used to initialize engine subsystems.
*/
#pragma once

#include "mg_VS\stdafx.h"
#include "mg\game\GameTypes.h"

class EngineConfigProperties
{
public:
	EngineConfigProperties()	{}
	~EngineConfigProperties() {}

	// HERE ARE ALL THE ENGINE CONFIGURATION SETTINGS. NOTE THAT
	// THESE SETTINGS ARE LOADED FROM THE ENGINE CONFIG FILE BUT
	// THAT THE GameConfig CLASS CAN LOAD OTHER SETTINGS AS WELL
	// FOR GAME-SPECIFIC STUFF AS NEEDED AS WELL

	// THESE ARE THE NAMES OF PROPERTIES AS LISTED IN THE CONFIG FILE
	wstring PROP_NAME_APPLICATION_NAME		= L"APPLICATION_NAME";
	wstring PROP_NAME_DEBUG_FILE			= L"DEBUG_FILE";
	wstring	PROP_NAME_OS					= L"OS";
	wstring	PROP_NAME_RENDER_LIB			= L"RENDER_LIB";
	wstring	PROP_NAME_IS_FULLSCREEN			= L"IS_FULLSCREEN";
	wstring	PROP_NAME_SCREEN_WIDTH			= L"SCREEN_WIDTH";
	wstring	PROP_NAME_SCREEN_HEIGHT			= L"SCREEN_HEIGHT";
	wstring	PROP_NAME_TARGET_FPS			= L"TARGET_FPS";
	wstring	PROP_NAME_DEBUG_TEXT_FONT_SIZE	= L"DEBUG_TEXT_FONT_SIZE";
	wstring	PROP_NAME_TOGGLE_OFFSET_Y		= L"TOGGLE_OFFSET_Y";
	wstring	PROP_NAME_FONT_COLOR_RED		= L"FONT_COLOR_RED";
	wstring	PROP_NAME_FONT_COLOR_GREEN		= L"FONT_COLOR_GREEN";
	wstring	PROP_NAME_FONT_COLOR_BLUE		= L"FONT_COLOR_BLUE";
	wstring	PROP_NAME_COLOR_KEY_RED			= L"COLOR_KEY_RED";
	wstring	PROP_NAME_COLOR_KEY_GREEN		= L"COLOR_KEY_GREEN";
	wstring	PROP_NAME_COLOR_KEY_BLUE		= L"COLOR_KEY_BLUE";
	wstring PROP_NAME_VIEWPORT_OFFSET_X		= L"VIEWPORT_OFFSET_X";
	wstring PROP_NAME_VIEWPORT_OFFSET_Y		= L"VIEWPORT_OFFSET_Y";

	// AND THESE ARE VALUES TO TEST AGAINST
	wstring	PROP_VALUE_WINDOWS	= L"WINDOWS";
	wstring	PROP_VALUE_MAC		= L"MAC";
	wstring	PROP_VALUE_LINUX	= L"LINUX";
	wstring	PROP_VALUE_DIRECTX	= L"DIRECTX";
	wstring PROP_VALUE_OPENGL	= L"OPENGL";
	wstring	PROP_VALUE_TRUE		= L"true";
};

