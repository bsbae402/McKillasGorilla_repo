#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\world\SparseLayer.h"
#include "mg\gsm\world\TiledLayer.h"
#include "mg\gsm\world\Tile.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gui\OverlayImage.h"
#include "mg\resources\importers\TMXMapImporter.h"
#include "tinyxml\tinystr.h";
#include "tinyxml\tinyxml.h";
#include "xmlfi\XMLFileImporter.h"

bool TMXMapImporter::load(wstring mapDir, wstring mapFile)
{
	this->dir = mapDir;
	mapLevelFilePath = dir + mapFile;
	bool success = loadMapInfo();
	if (!success) return false;
	success = buildWorldFromInfo();
	if (!success) return false;

	// WE'VE LOADED THE WORLD SO NOW
	// LET'S CLEAN UP
	tileSetInfos.clear();
	list<LayerInfo*>::iterator it = layerInfos.begin();
	while (it != layerInfos.end())
	{
		list<LayerInfo*>::iterator tempIt = it;
		it++;
		LayerInfo *layerInfoToDelete = (*tempIt);
		delete layerInfoToDelete;
	}
	layerInfos.clear();
	return true;
}

bool TMXMapImporter::loadMapInfo()
{
	// LOAD THE XML DOC
	const char *charPath = xmlReader.newCharArrayFromWstring(mapLevelFilePath);
	TiXmlDocument doc(charPath);
	delete charPath;
	bool loadOkay = doc.LoadFile();
	if (loadOkay)
	{
		TiXmlElement *pElem = doc.FirstChildElement();
		string eName;
		if (pElem)
		{
			eName = pElem->Value();
			if (strcmp(eName.c_str(), MAP_ELEMENT.c_str()) == 0)
			{
				// WHAT'S THE TYPE OF MAP?
				const char* orientation = pElem->Attribute(ORIENTATION_ATT.c_str());
				if (strcmp(orientation, ORTHOGONAL_VAL.c_str()) == 0)
				{
					mapType = MapType::ORTHOGONAL_MAP;
					return loadOrthographicMap(pElem);
				}
				else
				{
					mapType = MapType::ISOMETRIC_MAP;
					
					// WE COULD LOAD AN ISOMETRIC MAP HERE

					return nullptr;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}



bool TMXMapImporter::loadOrthographicMap(const TiXmlElement *pElem)
{
	this->width = xmlReader.extractIntAtt(pElem, WIDTH_ATT);
	this->height = xmlReader.extractIntAtt(pElem, HEIGHT_ATT);
	this->tileWidth = xmlReader.extractIntAtt(pElem, TILEWIDTH_ATT);
	this->tileHeight = xmlReader.extractIntAtt(pElem, TILEHEIGHT_ATT);

	const TiXmlNode *node = pElem->FirstChild();
	while (node)
	{
		string eName = node->Value();
		if (strcmp(eName.c_str(), TILESET_ELEMENT.c_str()) == 0)
		{
			loadTileSetInfo(node);
		}
		else if (strcmp(eName.c_str(), IMAGELAYER_ELEMENT.c_str()) == 0)
		{
			loadImageLayerInfo(node);
		}
		else if (strcmp(eName.c_str(), LAYER_ELEMENT.c_str()) == 0)
		{
			loadTiledLayerInfo(node);
		}
		else if (strcmp(eName.c_str(), OBJECT_GROUP_ELEMENT.c_str()) == 0)
		{
			loadSparseLayerInfo(node);
		}
		node = node->NextSibling();
	}
	return true;
}

void TMXMapImporter::loadImageLayerInfo(const TiXmlNode *node)
{
	const TiXmlElement *element = node->ToElement();
	ImageLayerInfo *imageLayerInfo = new ImageLayerInfo();
	if (element->Attribute(NAME_ATT.c_str()) == NULL) return;
	else imageLayerInfo->name = xmlReader.extractCharAtt(element, NAME_ATT);
	// WE DON'T ACTUALLY CARE ABOUT THE TMX WIDTH AND HEIGHT FOR THIS

	// NOW GET THE IMAGE ELEMENT
	const TiXmlNode *childNode = node->FirstChild();
	if (childNode->ToElement()->Attribute(SOURCE_ATT.c_str()) == NULL) return;
	else imageLayerInfo->imageSource = xmlReader.extractCharAtt(childNode->ToElement(), SOURCE_ATT);

	// AND NOW GET IMAGE LAYER PROPERTIES
	childNode = childNode->ToElement()->NextSibling();
	if (node != NULL)
	{
		string eName = childNode->Value();
		if (strcmp(eName.c_str(), PROPERTIES_ELEMENT.c_str()) == 0)
		{
			const TiXmlNode *grandchildNode = childNode->FirstChild();
			while (grandchildNode != NULL)
			{
				const TiXmlElement *grandchildElement = grandchildNode->ToElement();
				string att = grandchildElement->Attribute(NAME_ATT.c_str());
				if (strcmp(att.c_str(), COLLIDABLE_ATT.c_str()) == 0)
				{
 					imageLayerInfo->collidable = xmlReader.extractBoolAtt(grandchildNode->ToElement(), VALUE_ATT);
				}
				else if (strcmp(att.c_str(), IMAGEHEIGHT_ATT.c_str()) == 0)
				{
					imageLayerInfo->imageheight= xmlReader.extractIntAtt(grandchildNode->ToElement(), VALUE_ATT);
				}
				else if (strcmp(att.c_str(), IMAGEWIDTH_ATT.c_str()) == 0)
				{
					imageLayerInfo->imagewidth = xmlReader.extractIntAtt(grandchildNode->ToElement(), VALUE_ATT);
				}
				grandchildNode = grandchildNode->NextSibling();
			}
		}
	}
	// OK, NOW LOAD THE LAYER
	imageLayerInfo->type = LayerType::IMAGE;
	layerInfos.push_back(imageLayerInfo);
}

void TMXMapImporter::loadSparseLayerInfo(const TiXmlNode *node)
{
	const TiXmlElement *element = node->ToElement();
	SparseLayerInfo *sparseLayerInfo = new SparseLayerInfo();
	sparseLayerInfo->name = xmlReader.extractCharAtt(element, NAME_ATT);
	
	// NOW GET THE PROPERTIES
	const TiXmlNode *propertiesNode = node->FirstChild();
	string eName = propertiesNode->Value();
	const TiXmlNode *dataNode = propertiesNode;
	if (strcmp(eName.c_str(), PROPERTIES_ELEMENT.c_str()) == 0)
	{
		const TiXmlNode *propNode = propertiesNode->FirstChild();
		// IF THE LAYER IS NOT SPECIFIED AS COLLIDABLE OR NOT,
		// DON'T EVEN LOAD IT
		const char *propName = propNode->Value();
		if (strcmp(propNode->Value(), PROPERTY_ELEMENT.c_str()) != 0)
			return;

		// OTHERWISE LOAD THE PROPERTIES, WHICH MUST INCLUDE collidable
		while (propNode != NULL)
		{
			const TiXmlElement *propElement = propNode->ToElement();
			string att = propElement->Attribute(NAME_ATT.c_str());
			if (strcmp(att.c_str(), COLLIDABLE_ATT.c_str()) == 0)
			{
				sparseLayerInfo->collidable = xmlReader.extractBoolAtt(propNode->ToElement(), VALUE_ATT);
			}
			propNode = propNode->NextSibling();
		}
		dataNode = propertiesNode->NextSibling();
	}
	// NOW LET'S LOAD THE OBJECTS (Sparse Tiles) 
	while (dataNode != NULL)
	{
		const TiXmlElement *objectElement = dataNode->ToElement();
		if (	(dataNode->ToElement()->Attribute(GID_ATT.c_str()) != NULL) &&
			(dataNode->ToElement()->Attribute(X_ATT.c_str()) != NULL) &&
			(dataNode->ToElement()->Attribute(Y_ATT.c_str()) != NULL))
		{
			sparseLayerInfo->gidS.push_back(xmlReader.extractIntAtt(objectElement, GID_ATT));
			sparseLayerInfo->xS.push_back(xmlReader.extractIntAtt(objectElement, X_ATT));
			sparseLayerInfo->yS.push_back(xmlReader.extractIntAtt(objectElement, Y_ATT));
		}
		dataNode = dataNode->NextSibling();
	}

	sparseLayerInfo->tileSetInfo = getTileSetForId(sparseLayerInfo->gidS.front());
	sparseLayerInfo->type = LayerType::SPARSE;
	layerInfos.push_back(sparseLayerInfo);
}


void TMXMapImporter::loadTiledLayerInfo(const TiXmlNode *node)
{
	const TiXmlElement *element = node->ToElement();
	TiledLayerInfo *tiledLayerInfo = new TiledLayerInfo();

	/// int tiledLayerIndex

	// NAME WIDTH AND HEIGHT ARE ALL REQUIRED
	if (element->Attribute(NAME_ATT.c_str()) == NULL) return;
	else tiledLayerInfo->name = xmlReader.extractCharAtt(element, NAME_ATT);
	if (element->Attribute(WIDTH_ATT.c_str()) == NULL) return;
	else tiledLayerInfo->width = xmlReader.extractIntAtt(element, WIDTH_ATT);
	if (element->Attribute(HEIGHT_ATT.c_str()) == NULL) return;
	else tiledLayerInfo->height = xmlReader.extractIntAtt(element, HEIGHT_ATT);

	// NOW GET THE PROPERTIES, IF THERE ARE ANY
	const TiXmlNode *propertiesNode = node->FirstChild();
	string eName = propertiesNode->Value();
	const TiXmlNode *dataNode = NULL;
	bool collidableSpecified = false;
	if (strcmp(eName.c_str(), PROPERTIES_ELEMENT.c_str()) == 0)
	{
		const TiXmlNode *propNode = propertiesNode->FirstChild();
		while (propNode != NULL)
		{
			const TiXmlElement *propElement = propNode->ToElement();
			string att = propElement->Attribute(NAME_ATT.c_str());
			//// COLLIDABLE_ATT	= "collidable". 
			//// The layers in the TMX file should have specified "collidable" attribute to set the layer class's collidable value.
			if (strcmp(att.c_str(), COLLIDABLE_ATT.c_str()) == 0)
			{
				//// VALUE_ATT = "value"
				//// collidable's type is bool. Look up the extractBoolAtt() function, and you will find
				//// that it compares the "true" string to the value. 
				//// Thus, put "true" string in the tmx file layer's collidable value if the layer is collidable
				tiledLayerInfo->collidable = xmlReader.extractBoolAtt(propNode->ToElement(), VALUE_ATT);
				collidableSpecified = true;
			}
			propNode = propNode->NextSibling();
		}
		dataNode = propertiesNode->NextSibling();
	}

	// BE DEFAULT A LAYER IS NOT COLLIDABLE
	if (!collidableSpecified)
		tiledLayerInfo->collidable = false;

	// NOW GET THE TILE DATA
	if (dataNode == NULL)
	{
		dataNode = node->FirstChild();
	}
	const TiXmlNode *tileNode = dataNode->FirstChild();
	while (tileNode)
	{
		const TiXmlElement *element = tileNode->ToElement();
		
		// GID IS REQUIRED
		if (element->Attribute(GID_ATT.c_str()) != NULL)
		{
			int gid = xmlReader.extractIntAtt(element, GID_ATT);
			tiledLayerInfo->gids.push_back(gid);
		}
		tileNode = tileNode->NextSibling();
	}
	tiledLayerInfo->tileSetInfo = getTileSetForId(tiledLayerInfo->gids[0]);
	tiledLayerInfo->type = LayerType::TILED;
	layerInfos.push_back(tiledLayerInfo);
}

void TMXMapImporter::loadTileSetInfo(const TiXmlNode *node)
{
	const TiXmlElement *element = node->ToElement();
	TileSetInfo tileSetInfo;

	// EVERY TILED SET NEEDS A FIRSTGID, NAME, TILEWIDTH, & TILEHEIGHT,
	// SO WE'LL REQUIRE THESE
	if (element->Attribute(FIRSTGID_ATT.c_str()) == NULL) return;
	else tileSetInfo.firstgid = xmlReader.extractIntAtt(element, FIRSTGID_ATT);
	if (element->Attribute(NAME_ATT.c_str()) == NULL) return;
	else tileSetInfo.name = xmlReader.extractCharAtt(element, NAME_ATT);
	if (element->Attribute(TILEWIDTH_ATT.c_str()) == NULL) return;
	else tileSetInfo.tilewidth = xmlReader.extractIntAtt(element, TILEWIDTH_ATT);
	if (element->Attribute(TILEHEIGHT_ATT.c_str()) == NULL) return;
	else tileSetInfo.tileheight = xmlReader.extractIntAtt(element, TILEHEIGHT_ATT);

	// THESE ARE OPTIONAL ATTRIBUTES, AND SO WE WON'T REQUIRE THEM
	if (element->Attribute(SPACING_ATT.c_str()) != NULL)
		tileSetInfo.spacing = xmlReader.extractIntAtt(element, SPACING_ATT);
	else
		tileSetInfo.spacing = 0;
	if (element->Attribute(MARGIN_ATT.c_str()) != NULL)
		tileSetInfo.margin = xmlReader.extractIntAtt(element, MARGIN_ATT);
	else
		tileSetInfo.margin = 0;

	// NOW GET THE IMAGE INFO
	node = node->FirstChild();
	element = node->ToElement();

	// ALL THREE ATTRIBUTES ARE REQUIRED
	if (element->Attribute(SOURCE_ATT.c_str()) == NULL) return;
	else tileSetInfo.sourceImage = xmlReader.extractCharAtt(element, SOURCE_ATT);
	if (element->Attribute(WIDTH_ATT.c_str()) == NULL) return;
	else tileSetInfo.sourceImageWidth = xmlReader.extractIntAtt(element, WIDTH_ATT);
	if (element->Attribute(HEIGHT_ATT.c_str()) == NULL) return;
	else tileSetInfo.sourceImageHeight = xmlReader.extractIntAtt(element, HEIGHT_ATT);

	// TRANSPARENCY MAY NOT BE PROVIDED
	if (element->Attribute(COLOR_KEY_ATT.c_str()) != NULL)
		tileSetInfo.transparencyColor = xmlReader.extractCharAtt(element, COLOR_KEY_ATT);

	tileSetInfos.push_back(tileSetInfo);
}

bool TMXMapImporter::buildWorldFromInfo()
{
	Game *game = Game::getSingleton();
	TextureManager *worldTextureManager = game->getGraphics()->getWorldTextureManager();
	if (mapType == MapType::ORTHOGONAL_MAP)
	{
		World *world = game->getGSM()->getWorld();

		// LET'S FIRST FIGURE OUT THE WORLD WIDTH AND HEIGHT
		calculateWorldDimensions();

		// AND MAKE THE WORLD DIMENSIONS THE
		// SIZE OF THE LARGEST LAYER
		world->setWorldWidth(largestLayerWidth);
		world->setWorldHeight(largestLayerHeight);

		// IT'S POSSIBLE THE TEXTURE MANAGER ALREADY HAS SOME
		// IMAGES, SO WE NEED TO KNOW HOW MANY IMAGES ARE ALREADY
		// THER TO OFFSET THE IDS OF ALL IMAGE REFERENCES
		unsigned int idOffset = worldTextureManager->getWStringTable()->getNumWStringsInTable();

		// NOW LOAD THE TILE SETS IN THE ORDER THEY WERE LISTED
		// INSIDE TMX FILE
		list<TileSetInfo>::iterator tsiIt = tileSetInfos.begin();
		while (tsiIt != tileSetInfos.end())
		{
			TileSetInfo tsi = (*tsiIt);
			wstring sourceImageW(tsi.sourceImage.begin(), tsi.sourceImage.end());
			bool success = worldTextureManager->loadTileSetFromTexture(dir, sourceImageW, tsi.tilewidth, tsi.tileheight, tsi.spacing, tsi.margin);
			if (!success) return false;
			tsiIt++;
		}

		// NOW LOAD ALL THE LAYERS
		list<LayerInfo*>::iterator it = layerInfos.begin();
		while (it != layerInfos.end())
		{
			LayerInfo *layer = (*it);

			if (layer->type == LayerType::IMAGE)
			{
				ImageLayerInfo *ili = (ImageLayerInfo*)layer;
				buildImageLayer(ili, idOffset);
			}
			else if (layer->type == LayerType::TILED)
			{
				TiledLayerInfo *tli = (TiledLayerInfo*)layer;
				//// -- here we build a tiled layer. 
				//// see loadTiledLayerInfo method to see how the TiledLayerInfo store the info.
				buildTiledLayer(tli, idOffset);
			}
			else if (layer->type == LayerType::SPARSE)
			{
				SparseLayerInfo *sli = (SparseLayerInfo*)layer;
				buildSparseLayer(sli, idOffset);
			}

			// ON TO THE NEXT LAYER
			it++;
		}
	}
	return true;
}

void TMXMapImporter::buildTiledLayer(TiledLayerInfo *tli, int idOffset)
{
	TiledLayer *tiledLayerToAdd = new TiledLayer(	tli->width,
													tli->height,
													tli->tileSetInfo->tilewidth,
													tli->tileSetInfo->tileheight,
													0,
													tli->collidable,
													largestLayerWidth,
													largestLayerHeight);
	Game *game = Game::getSingleton();
	game->getGSM()->getWorld()->addLayer(tiledLayerToAdd);

	// WE HAVE TO ADD ALL THE TILES
	int row = 0;
	int col = 0;
	int uncollidableIndex = tli->tileSetInfo->firstgid;
	for (unsigned int i = 0; i < tli->gids.size(); i++)
	{
		Tile *tileToAdd = new Tile();
		tileToAdd->textureID = tli->gids[i] + idOffset - 1;
		if (tli->gids[i] == uncollidableIndex)
			tileToAdd->collidable = false;
		else
			tileToAdd->collidable = tli->collidable;
		tiledLayerToAdd->addTile(tileToAdd);
	}
}

void TMXMapImporter::buildImageLayer(ImageLayerInfo *ili, int idOffset)
{
	TiledLayer *imageLayerToAdd = new TiledLayer(	1,
													1,
													ili->imagewidth,
													ili->imageheight,
													0,
													ili->collidable,
													largestLayerWidth,
													largestLayerHeight);
	Game *game = Game::getSingleton();
	game->getGSM()->getWorld()->addLayer(imageLayerToAdd);
	TextureManager *textureManager = game->getGraphics()->getWorldTextureManager();
	Tile *imageTile = new Tile();
	imageTile->collidable = ili->collidable;
	wstring imageSourceW(ili->imageSource.begin(), ili->imageSource.end());
	imageTile->textureID = textureManager->loadTexture(dir + imageSourceW) + idOffset - 1;
	imageLayerToAdd->addTile(imageTile);
}

void TMXMapImporter::buildSparseLayer(SparseLayerInfo *sli, int idOffset)
{
	Game *game = Game::getSingleton();
	// AND NOW THE SPARSE LAYERS
	SparseLayer *sparseLayer = new SparseLayer();
	game->getGSM()->getWorld()->addLayer(sparseLayer);

	list<int>::iterator xIt = sli->xS.begin();
	list<int>::iterator yIt = sli->yS.begin();
	list<int>::iterator gidIt = sli->gidS.begin();
	while (xIt != sli->xS.end())
	{
		int x = (*xIt);
		int y = (*yIt);
		int gid = (*gidIt);
		OverlayImage *sparseTileToAdd = new OverlayImage();
		sparseTileToAdd->alpha = 255;
		sparseTileToAdd->x = x;
		sparseTileToAdd->y = y;
		sparseTileToAdd->z = 0;
		sparseTileToAdd->imageID = gid + idOffset - 1;
		sparseTileToAdd->width = sli->tileSetInfo->tilewidth;
		sparseTileToAdd->height = sli->tileSetInfo->tilewidth;
		sparseTileToAdd->rotationInRadians = 0.0f;
		sparseLayer->addTile(sparseTileToAdd);

		xIt++;
		yIt++;
		gidIt++;
	}
}

TileSetInfo* TMXMapImporter::getTileSetForId(int id)
{
	list<TileSetInfo>::iterator it = tileSetInfos.begin();
	while (it != tileSetInfos.end())
	{
		TileSetInfo *tsi = &(*it);
		int columns = tsi->sourceImageWidth/tsi->tilewidth;
			int rows = tsi->sourceImageHeight/tsi->tileheight;
		int numTiles = rows * columns;
		if ((id >= tsi->firstgid) && (id < (tsi->firstgid + numTiles)))
		{
			return tsi;
		}
		it++;
	}
	return NULL;
}

void TMXMapImporter::calculateWorldDimensions()
{
	// GO THROUGH EACH LAYER INFO
	list<LayerInfo*>::iterator it = layerInfos.begin();
	while (it != layerInfos.end())
	{
		LayerInfo *layer = *it;

		// IT'S AN IMAGE LAYER
		if (layer->type == LayerType::IMAGE)
		{
			ImageLayerInfo *ili = (ImageLayerInfo*)layer;
			if (ili->imagewidth > largestLayerWidth)
				largestLayerWidth = ili->imagewidth;
			if (ili->imageheight > largestLayerHeight)
				largestLayerHeight = ili->imageheight;
		}
		// IT'S A TILED LAYER
		else if (layer->type == LayerType::TILED)
		{
			TiledLayerInfo *tli = (TiledLayerInfo*)layer;
			int layerWidth = tli->width * tli->tileSetInfo->tilewidth;
			if (layerWidth > largestLayerWidth)
				largestLayerWidth = layerWidth;
			int layerHeight = tli->height * tli->tileSetInfo->tileheight;
			if (layerHeight > largestLayerHeight)
				largestLayerHeight = layerHeight;
		}
		// IT'S A SPARSE LAYER
		else if (layer->type == LayerType::SPARSE)
		{
			SparseLayerInfo *sli = (SparseLayerInfo*)layer;

			// NOTE THAT WE DON'T CARE ABOUT THE MIN X & Y FOR A SPARSE LAYER
			// SINCE WE'LL SAY THE MIN X & Y ARE ALWAYS 0
			list<int>::iterator xIt = sli->xS.begin();
			list<int>::iterator yIt = sli->yS.begin();
			while (xIt != sli->xS.end())
			{
				int rightEdge = (*xIt) + sli->tileSetInfo->tilewidth;
				int bottomEdge = (*yIt) + sli->tileSetInfo->tileheight;
				if (rightEdge > largestLayerWidth)
					largestLayerWidth = rightEdge;
				if (bottomEdge > largestLayerHeight)
					largestLayerHeight = bottomEdge;
				xIt++;
				yIt++;
			}
		}
		// ON TO THE NEXT LAYER
		it++;
	}
}


bool TMXMapImporter::unload()
{
	return false;
}