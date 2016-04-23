/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	World.cpp

	See World.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\graphics\GameGraphics.h"
#include "mg\graphics\RenderList.h"
#include "mg\graphics\TextureManager.h"
#include "mg\gsm\sprite\SpriteManager.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\world\World.h"
#include "mg\gui\Viewport.h"
#include "mg\gsm\world\TiledLayer.h"
#include <vector>

/*
	World - Default Constructor, it constructs the layers
	vector, allowing new layers to be added.
*/
World::World()	
{
	layers = new vector<WorldLayer*>();
	worldWidth = 0;
	worldHeight = 0;
}

/*
	~World - This destructor will remove the memory allocated
	for the layer vector.
*/
World::~World()	
{
	delete layers;
}

/*
	addLayer - This method is how layers are added to the World.
	These layers might be TiledLayers, SparseLayers, or 
	IsometricLayers, all of which are child classes of WorldLayer.
*/
void World::addLayer(WorldLayer *layerToAdd)
{
	layers->push_back(layerToAdd);
}

/*
	addWorldRenderItemsToRenderList - This method sends the render
	list and viewport to each of the layers such that they
	may fill it with RenderItems to draw.
*/
void World::addWorldRenderItemsToRenderList()
{
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	GameGUI *gui = game->getGUI();
	if (gsm->isWorldRenderable())
	{
		GameGraphics *graphics = game->getGraphics();
		RenderList *renderList = graphics->getWorldRenderList();
		Viewport *viewport = gui->getViewport();
		for (unsigned int i = 0; i < layers->size(); i++)
		{
			layers->at(i)->addRenderItemsToRenderList(	renderList,
														viewport);
		}
	}
}


int World::getCollidableGridColumns()
{
	return getLayers()->front()->getColumns();
	//return  getWorldWidth() / 64;
}

int World::getCollidableGridRows()
{
	return getLayers()->front()->getRows();
	//return getWorldHeight() / 64;
}

bool World::overlapsCollidableTiles(int centerX, int centerY, int nodeWidth, int nodeHeight)
{
	return false;
}



bool World::isInsideCollidableTile(int targetX, int targetY)
{
	// size of collidable layer's tiles
	int cellWidth = collidableLayer->getTileWidth();
	int cellHeight = collidableLayer->getTileHeight();
	int halfCellWidth = cellWidth / 2;
	int halfCellHeight = cellHeight / 2;

	// x=96 -> (x-32)=64 -> (x-32)/64 = 1
	//x=160 -> (x-32)=128 -> (x-32)/64 = 2
	int columnIdx = (targetX - halfCellWidth) / cellWidth;
	int rowIdx = (targetY - halfCellHeight) / cellHeight;

	// just found that the getTile() wants us to consider the row/col starts from [0] to [49]
	// like the array of programmings. Good.
	Tile *targetTile = collidableLayer->getTile(rowIdx, columnIdx);
	if (targetTile->collidable)
		return true;
	else
		return false;
}

/*
	clear - This method removes all data from the World. It should
	be called first when a level is unloaded or changed. If it
	is not called, an application runs the risk of having lots
	of extra data sitting around that may slow the progam down.
	Or, if the world thinks a level is still active, it might add
	items to the render list using image ids that have already been
	cleared from the GameGraphics' texture manager for the world.
	That would likely result in an exception.
*/
void World::unloadWorld()
{
	// unload newly implemented World properties of Rebelle
	collidableLayer = nullptr;

	// GO THROUGH AND DELETE ALL THE LAYERS
	vector<WorldLayer*>::iterator it = layers->begin();
	while (it != layers->end())
	{
		vector<WorldLayer*>::iterator tempIt = it;
		it++;
		WorldLayer *layerToDelete = (*tempIt);
		delete layerToDelete;
	}
	layers->clear();
	worldWidth = 0;
	worldHeight = 0;
}

/*
	update - This method should be called once per frame. Note that
	the World is for static objects, so we don't have anything
	to update as/is. But, should the need arise, one could add non-
	collidable layers to a game world and update them here. For
	example, particle systems.
*/
void World::update()
{
	// NOTE THAT THIS METHOD IS NOT IMPLEMENTED BUT COULD BE
	// SHOULD YOU WISH TO ADD ANY NON-COLLIDABLE LAYERS WITH
	// DYNAMIC CONTENT OR PARTICLE SYSTEMS
}