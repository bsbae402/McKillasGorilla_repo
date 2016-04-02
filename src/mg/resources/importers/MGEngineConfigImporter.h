#pragma once

#include "mg_VS\stdafx.h"
#include "mg\resources\MGImporter.h"

// XML ELEMENTS
static const string		GAME_ELEMENT = "game";
static const string		GAME_PROPERTIES_LIST_ELEMENT = "game_properties_list";
static const string		GAME_PROPERTY_ELEMENT = "game_propertyh";
static const string		LEVELS_LIST_ELEMENT = "levels_list";
static const string		LEVEL_ELEMENT = "level";
static const string		GUI_ELEMENT = "gui";

// XML ATTRIBUTES
static const string		PROPERTY_NAME_ATT = "name";
static const string		PROPERTY_VALUE_ATT = "value";
static const string		TYPE_ATT = "type";
static const string		DIR_ATT = "dir";
static const string		FILE_ATT = "file";

class MGEngineConfigImporter : public MGImporter
{
private:
	// FOR STORING CONFIGURATION FILE PROPERTIES
	map<wstring, wstring> properties;
	void *propertyTypes;

public:
	void putProperty(wstring propertyName, wstring propertyValue)
	{
		properties[propertyName] = propertyValue;
	}

	wstring getProperty(wstring propertyName)
	{
		return properties[propertyName];
	}

	wstring getProperty(string propertyName)
	{
		wstring wPropName(propertyName.begin(), propertyName.end());
		return properties[wPropName];
	}

	void* getPropertyTypes()
	{
		return propertyTypes;
	}

	void setPropertyTypes(void* initPropertyTypes)
	{
		propertyTypes = initPropertyTypes;
	}

	// FOR LOADING PROPERTIES/CONFIG FILES
	bool load(wstring gameFileDir, wstring gameFile);
	bool unload();
};