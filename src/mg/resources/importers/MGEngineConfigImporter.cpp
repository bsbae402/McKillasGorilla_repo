#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\resources\EngineConfigProperties.h"
#include "mg\resources\GameResources.h"
#include "mg\resources\MGImporter.h"
#include "mg\resources\importers\MGEngineConfigImporter.h"
#include "tinyxml\tinystr.h";
#include "tinyxml\tinyxml.h";
#include "xmlfi\XMLFileImporter.h"

bool MGEngineConfigImporter::load(wstring gameFileDir, wstring gameFile)
{
	// WE'RE LOADING SOME PROPERTIES INTO OUR RESOURCE MANAGER
	Game *game = Game::getSingleton();
	GameResources *resources = game->getResources();

	// LOAD THE XML DOC
	XMLFileImporter xmlReader;
	wstring fullPath = gameFileDir + gameFile;
	const char *charPath = xmlReader.newCharArrayFromWstring(fullPath);
	TiXmlDocument doc(charPath);

	// DID IT LOAD SUCCESSULLY?
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		// FIRST WE HAVE THE GAME NODE, FROM WHICH WE'LL GET EVERYTHING ELSE
		const TiXmlElement *pElem = doc.FirstChildElement();

		// WE NEED THE GAME NODE'S 3 CHILDREN

		// FIRST THE PROPERTIES LIST
		const TiXmlElement *propertiesList = pElem->FirstChildElement();
		const TiXmlElement *property = propertiesList->FirstChildElement();
		while (property != nullptr)
		{
			// GET THE PROPERTY NAME
			const char* propNameText = property->Attribute(PROPERTY_NAME_ATT.c_str());
			string propName = string(propNameText);
			wstring propertyName = wstring(propName.begin(), propName.end());

			// AND THE PROPERTY VALUE
			const char* propValueText = property->Attribute(PROPERTY_VALUE_ATT.c_str());
			string propValue = string(propValueText);
			wstring propertyValue = wstring(propValue.begin(), propValue.end());

			// KEEP THE PROPERTY
			properties[propertyName] = propertyValue;

			// AND ONTO THE NEXT PROPERTY
			property = property->NextSiblingElement();
		}

		// THEN THE LEVELS LIST
		const TiXmlElement *levelsList = propertiesList->NextSiblingElement();
		const TiXmlElement *level = levelsList->FirstChildElement();
		while (level != nullptr)
		{
			// GET THE LEVEL NAME
			const char* nameText = level->Attribute(PROPERTY_NAME_ATT.c_str());
			string name = string(nameText);
			wstring wName = wstring(name.begin(), name.end());

			// GET THE LEVEL FILE'S DIRECTORY
			const char* dirText = level->Attribute(DIR_ATT.c_str());
			string dir = string(dirText);
			wstring wDir = wstring(dir.begin(), dir.end());

			// AND THE PROPERTY VALUE
			const char* fileText = level->Attribute(FILE_ATT.c_str());
			string file = string(fileText);
			wstring wFile = wstring(file.begin(), file.end());

			// KEEP THE LEVEL
			GameStateManager *gsm = game->getGSM();
			gsm->addLevel(wName, wDir, wFile);

			// AND ONTO THE NEXT Level
			level = level->NextSiblingElement();
		}

		// AND FINALLY THE GUI
		const TiXmlElement *gui = levelsList->NextSiblingElement();

		// GET THE GUI FILE'S DIRECTORY
		const char* guiDirText = gui->Attribute(DIR_ATT.c_str());
		string guiDir = string(guiDirText);
		wstring wGuiDir = wstring(guiDir.begin(), guiDir.end());

		// AND THE PROPERTY VALUE
		const char* guiFileText = gui->Attribute(FILE_ATT.c_str());
		string guiFile = string(guiFileText);
		wstring wGuiFile = wstring(guiFile.begin(), guiFile.end());

		// KEEP THE GUI FILE FOR WHEN IT GETS LOADED
		resources->setGuiFile(wGuiDir + wGuiFile);

		// WE'RE DONE
		return true;
	}
	else
		return false;
}



bool MGEngineConfigImporter::unload()
{
	return false;
}

/*bool MGEngineConfigImporter::loadPropertiesList(const TiXmlNode *node)
{
Game *game = Game::getSingleton();
GameConfig *config = game->getConfig();

const TiXmlNode *propertyNode = node->FirstChild();
map<string, vector<Pose>> animationStates;
while (animationStateNode)
{
// FIRST GET THE STATE NAME
const TiXmlNode *stateNode = animationStateNode->FirstChild();
const TiXmlElement *stateElement = stateNode->ToElement();
string state = stateElement->FirstChild()->Value();
bool success = loadAnimationStateInfo(stateNode, &animationStates, state);
if (!success) return false;
animationStateNode = animationStateNode->NextSibling();
}
spriteTypesAnimationsLists[spriteTypeName] = animationStates;
return true;
}
*/
