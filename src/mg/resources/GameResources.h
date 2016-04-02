#pragma once

#include "mg_VS\stdafx.h"
#include "mg\resources\MGImporter.h"
#include "xmlfi\XMLFileImporter.h"

class GameResources
{
private:	
	// THIS IS A SETUP FILE FOR LOADING THE GUI
	wstring guiFile;

	// THESE IMPORTERS ARE FOR LOADING GAME & LEVEL DATA
	MGImporter *engineConfigImporter;
	MGImporter *levelImporter;
	MGImporter *mapImporter;
	MGImporter *spriteTypeImporter;

public:
	// NOTHING TO INITIALIZE, WE'LL LEAVE THE DEFAULT CONSTRUCTION/DESTRUCTION OF THE
	// IMPORTERS UP TO THE startUp/shutDown METHODS
	GameResources()		{}
	~GameResources()	{}

	// INLINED ACCESSOR METHODS
	MGImporter*		getEngineConfigImporter()	{ return engineConfigImporter;	}
	MGImporter*		getLevelImporter()			{ return levelImporter;			}
	MGImporter*		getMapImporter()			{ return mapImporter;			}
	MGImporter*		getSpriteTypeImporter()		{ return spriteTypeImporter;	}
	wstring			getGUIFile()				{ return guiFile;				}

	// INLINED MUTATOR METHODS
	void setEngineConfigImporter(MGImporter *initEngineConfigImporter)
	{
		engineConfigImporter = initEngineConfigImporter;
	}
	void setLevelImporter(MGImporter *initLevelImporter)
	{
		levelImporter = initLevelImporter;
	}
	void setMapImporter(MGImporter *initMapImporter)
	{
		mapImporter = initMapImporter;
	}
	void setSpriteTypeImporter(MGImporter *initSpriteTypeImporter)
	{
		spriteTypeImporter = initSpriteTypeImporter;
	}
	void setGuiFile(wstring initGuiFile)
	{
		guiFile = initGuiFile;
	}

	void startUp();
	void shutDown();
};
