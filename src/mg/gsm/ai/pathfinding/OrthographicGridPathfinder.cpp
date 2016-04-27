#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\ai\pathfinding\GridPathfinder.h"
#include "mg\gsm\ai\pathfinding\OrthographicGridPathfinder.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\state\GameStateManager.h"
#include "mg\gsm\world\World.h"
#include "mg\gsm\world\WorldLayer.h"
#include "mg\gsm\world\TiledLayer.h"
#include "mg\gsm\physics\AABB.h"

OrthographicGridPathfinder::OrthographicGridPathfinder(Game *initGame)
{
	game = initGame;

	// WE'LL DETERMINE THE NUMBER OF ROWS AND COLUMNS BASED ON THE FIRST
	// COLLIDABLE TILED LAYER WE FIND
	World *world = game->getGSM()->getWorld();
	numColumns = world->getCollidableGridColumns();
	numRows = world->getCollidableGridRows();
	initGrid();

	selectedPathfindingCell.setCenterX(0.0f);
	selectedPathfindingCell.setCenterY(0.0f);
	selectedPathfindingCell.setWidth(getGridWidth());
	selectedPathfindingCell.setHeight(getGridHeight());
}

void OrthographicGridPathfinder::buildPath(	list<PathNode> *pathToFill,
											float startX, float startY,
											float endX, float endY)
{
	// SO BUILD THE PATH
	pathToFill->clear();
	vector<bool> *pathGridPointer = &pathGrid;
	GameStateManager *gsm = game->getGSM();
	World *world = gsm->getWorld();
	int gridWidth = getGridWidth();
	int gridHeight = getGridHeight();

	// DETERMINE THE START COLUMN AND START ROW
	int startColumn = (int)(startX/gridWidth);
	int startRow = (int)(startY/gridHeight);

	// DETERMINE THE END COLUMN AND END ROW
	int endColumn = (int)(endX/gridWidth);
	int endRow = (int)(endY/gridHeight);

	// IF THE DESTINATION IS A COLLIDABLE TILE LOCATION
	// THEN EXIT
	int endIndex = getGridIndex(endColumn, endRow);
	bool endIndexIsWalkable = pathGrid[getGridIndex(endColumn, endRow)];
	if (!endIndexIsWalkable)
		return;

	map<int, PathNode> openNodes;
	map<int, PathNode> closedNodes;
	bool pathFound = false;
	bool nodesToAdd[9];

	PathNode startNode;
	startNode.column = startColumn;
	startNode.row = startRow;
	startNode.parentNode = NULL;
	startNode.G = 0;

	PathNode endNode;
	endNode.column = endColumn;
	endNode.row = endRow;
	endNode.parentNode = NULL;
	startNode.H = calculateH(startNode, &endNode);
	int nodeIndex = getGridIndex(startColumn, startRow);
	openNodes[nodeIndex] = startNode;

	while (!pathFound)
	{
		// IF THERE ARE NO MORE NODES TO SEARCH THROUGH TO FIND
		// OUR DESTINATION THEN WE'RE DONE
		if (openNodes.size() == 0)
		{
			pathToFill->clear();
			return;
		}

		// FIRST GET THE CLOSEST NODE FROM THE OPEN LIST
		PathNode *foundNode = findCheapestNode(&openNodes);
		PathNode cheapestNode;
		cheapestNode.column = foundNode->column;
		cheapestNode.row = foundNode->row;
		cheapestNode.G = foundNode->G;
		cheapestNode.H = foundNode->H;
		cheapestNode.parentNode = foundNode->parentNode;
		openNodes.erase(getGridIndex(cheapestNode.column, cheapestNode.row));

		// IS THE CHEAPEST NODE THE DESTINATION?
		if ((cheapestNode.column == endNode.column) && (cheapestNode.row == endNode.row))
		{
			// WE'VE REACHED THE DESTINATION
			pathFound = true;
			PathNode *traveller = &cheapestNode;
			while (traveller != NULL)
			{
				PathNode nodeToAdd;
				nodeToAdd.column = traveller->column;
				nodeToAdd.row = traveller->row;
				pathToFill->push_front(nodeToAdd);
				traveller = traveller->parentNode;
			}
		}
		else
		{
			// WE'LL NEED TO LOOK AT THE CHEAPEST NODE'S NEIGHBORS
			// FIRST LET'S PUT IT INTO THE CLOSED LIST SO WE DON'T
			// END UP IN AN INFINITELY CIRCULAR LOOP
			closedNodes[getGridIndex(cheapestNode.column, cheapestNode.row)] = cheapestNode;
			PathNode *nodeJustAdded = &closedNodes[getGridIndex(cheapestNode.column, cheapestNode.row)];

			// NOW FIGURE OUT WHICH NEIGHBORS MIGHT BE USABLE
			findNeighborsToCheck(nodesToAdd, nodeJustAdded, &closedNodes);

			// NOW ADD THE NEIGHBORS TO OUR OPEN LIST
			addNeighbors(nodesToAdd, nodeJustAdded, &endNode, &openNodes, &closedNodes);
		}
	}
	PathNode lastNode = pathToFill->back();
	destinationPathfindingCell.setCenterX(getColumnCenterX(lastNode.column));
	destinationPathfindingCell.setCenterY(getRowCenterY(lastNode.row));
	destinationPathfindingCell.setWidth(this->getGridWidth());
	destinationPathfindingCell.setHeight(this->getGridHeight());
}

void OrthographicGridPathfinder::removeNodeFromList(PathNode *nodeToRemove, list<PathNode> *nodeList)
{
	list<PathNode>::iterator it = nodeList->begin();
	while (it != nodeList->end())
	{
		PathNode testNode = (*it);
		if ((testNode.column == nodeToRemove->column) && (testNode.row == nodeToRemove->row))
		{
			nodeList->erase(it);
			return;
		}
		it++;
	}
}

PathNode* OrthographicGridPathfinder::findCheapestNode(map<int, PathNode> *openNodes)
{
	PathNode *minNode = NULL;
	if (openNodes->size() != 0)
	{
		map<int, PathNode>::iterator it = openNodes->begin();
		while (it != openNodes->end())
		{
			PathNode *testNode = &(it->second);
			if (minNode == NULL)
				minNode = testNode;
			else if ((testNode->G+testNode->H) < (minNode->G + minNode->H))
				minNode = testNode;
			it++;
		}
	}
	return minNode;
}

void OrthographicGridPathfinder::addNeighbors(bool *nodesToAdd, PathNode *centerNode, PathNode *destination, map<int, PathNode> *openNodes, map<int, PathNode> *closedNodes)
{
	int adjacencyIndex = 0;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			bool shouldBeAdded = nodesToAdd[adjacencyIndex];
			if (shouldBeAdded)
			{
				int col = i + centerNode->column - 1;
				int row = j + centerNode->row - 1;
				PathNode testNode;
				testNode.column = col;
				testNode.row = row;
				testNode.parentNode = centerNode;
				testNode.G = calculateG(testNode);
				testNode.H = calculateH(testNode, destination);

				// BEFORE WE ADD IT, CHECK TO SEE IF WE'VE ALREADY
				// FOUND A FASTER WAY OF GETTING TO THIS NODE THAT
				// IS IN THE OPEN LIST
				int index = getGridIndex(testNode.column, testNode.row);
				if (openNodes->find(index) != openNodes->end())
				{ 
					PathNode *dup = &openNodes->at(index);
					if ((testNode.G+testNode.H) < (dup->G + dup->H))
					{
						// IT'S BETTER THAN WHAT'S ALREADY THERE, SO
						// UPDATE THE ONE THAT'S ALREADY THERE
						dup->column = testNode.column;
						dup->row = testNode.row;
						dup->G = testNode.G;
						dup->H = testNode.H;
						dup->parentNode = testNode.parentNode;
					}
				}
				else
					(*openNodes)[index] = testNode;
			}
			adjacencyIndex++;
		}
	}
}

PathNode* OrthographicGridPathfinder::findDupNodeInList(PathNode targetNode, list<PathNode> *nodeList)
{
	list<PathNode>::iterator it = nodeList->begin();
	while (it != nodeList->end())
	{
		PathNode *testNode = &(*it);
		if ((testNode->column == targetNode.column) && (testNode->row == targetNode.row))
			return testNode;
		it++;
	}
	return NULL;
}

void OrthographicGridPathfinder::findNeighborsToCheck(	bool *adjacencies,
														PathNode *nodeToConsider,
														map<int, PathNode> *closedNodes)
{
	int ntcCol = nodeToConsider->column;
	int ntcRow = nodeToConsider->row;
	int adjacencyIndex = 0;
	for (int i = ntcCol-1; i <= ntcCol+1; i++)
	{
		for (int j = ntcRow-1; j <= ntcRow+1; j++)
		{
			if ((i >= 0) && (j >= 0) && (i < numColumns) && (j < numRows))
			{
				PathNode nodeToAdd;
				nodeToAdd.column = i;
				nodeToAdd.row = j;
				nodeToAdd.parentNode = nodeToConsider;

				int index = getGridIndex(nodeToAdd.column, nodeToAdd.row);
				if (closedNodes->find(index) == closedNodes->end())
				{
					// TEST TO SEE IF IT'S A COLLIDABLE TILE
					adjacencies[adjacencyIndex] = pathGrid[nodeToAdd.column + (nodeToAdd.row * numColumns)];
				}
				else
					adjacencies[adjacencyIndex] = false;
			}
			adjacencyIndex++;
		}
	}

	// NOW UPDATE THE ADJACENCIES
	if ((adjacencies[getAdjacencyIndex(0, 1)] == false) || (adjacencies[getAdjacencyIndex(1, 0)] == false))
		adjacencies[getAdjacencyIndex(0, 0)] = false;
	if ((adjacencies[getAdjacencyIndex(1, 0)] == false) || (adjacencies[getAdjacencyIndex(2, 1)] == false))
		adjacencies[getAdjacencyIndex(2, 0)] = false;
	if ((adjacencies[getAdjacencyIndex(0, 1)] == false) || (adjacencies[getAdjacencyIndex(1, 2)] == false))
		adjacencies[getAdjacencyIndex(0, 2)] = false;
	if ((adjacencies[getAdjacencyIndex(1, 2)] == false) || (adjacencies[getAdjacencyIndex(2, 1)] == false))
		adjacencies[getAdjacencyIndex(2, 2)] = false;

	// FINALLY, DON'T CHECK THE CENTER NODE EVER
	adjacencies[getAdjacencyIndex(1, 1)] = false;
}


int OrthographicGridPathfinder::getAdjacencyIndex(int col, int row)
{
	int index = (row * 3) + col;
	if ((index < 0) || (index > 8))
		cout << "HUH?";
	return index;
}

int OrthographicGridPathfinder::calculateG(PathNode node)
{
	int G = 0;
	if (node.parentNode != NULL)
	{
		G = node.parentNode->G;
		// IF IT'S THE SAME NODE, IT HAS THE SAME G
		if ((node.column == node.parentNode->column) && (node.row == node.parentNode->row))
			return G;

		// LEFT OR RIGHT
		if (node.column == node.parentNode->column)
			G += COLUMN_COST;
		// TOP OR BOTTOM
		else if (node.row == node.parentNode->row)
			G += ROW_COST;
		// MUST BE DIAGONAL
		else
			G += DIAGONAL_COST;
	}
	return G;
}


int OrthographicGridPathfinder::calculateH(PathNode start, PathNode *end)
{
	// HOW MANY DIAGONAL LINKS?
	int colDiff = abs(start.column - end->column);
	int rowDiff = abs(start.row - end->row);
	int numDiagonals = min(colDiff, rowDiff);
	int totalDiagonalCost = numDiagonals * DIAGONAL_COST;
	int totalColCost = (colDiff - numDiagonals) * COLUMN_COST;
	int totalRowCost = (rowDiff - numDiagonals) * ROW_COST;
	return totalDiagonalCost + totalColCost + totalRowCost;
}

bool OrthographicGridPathfinder::containsPathNode(list<PathNode> *path, PathNode testNode)
{
	list<PathNode>::iterator it = path->begin();
	while (it != path->end())
	{
		PathNode nodeInV = (*it);
		if ((nodeInV.column == testNode.column)
			&& (nodeInV.row == testNode.row))
			return true;
		it++;
	}
	return false;
}

void OrthographicGridPathfinder::updatePath(AnimatedSprite *sprite)
{
	list<PathNode> *currentPath = sprite->getCurrentPathToFollow();
	list<PathNode>::iterator currentNode = sprite->getCurrentPathNode();
	Game *game = Game::getSingleton();

	// IF THERE IS A PATH WE SHOULD BE FOLLOWING, FOLLOW IT
	if ((currentPath->size() > 0) && (currentNode != currentPath->end()))
	{
		// WE MIGHT ALSO DO A LITTLE COLLISION AVOIDANCE FOR
		// OTHER SPRITES VIA PHYSICS, BUT FOR NOW, LET'S JUST
		// DO SIMPLE POINT TO POINT PATHFINDING

		// FIRST, WE WANT TO SEE IF WE ARE OVERLAPPING THE FIRST
		// NODE IN THE PATH. IF WE ARE, THEN WE SHOULD MOVE ON TO THE 
		// NEXT NODE. IF NOT, WE SHOULD ADJUST OUR VELOCITY TO 
		// HEAD IN THAT DIRECTION
		PathNode node = *currentNode;
		if(hasReachedNode(sprite, node))
		{	
			sprite->advanceCurrentPathNode();
			if (sprite->hasReachedDestination())
			{
				sprite->getPhysicalProperties()->setVelocity(0.0f, 0.0f);
				sprite->clearPath();

				int temp = sprite->getPhysicalProperties()->getFinalX();
				sprite->getPhysicalProperties()->setFinalX(sprite->getPhysicalProperties()->getStartX());
				sprite->getPhysicalProperties()->setStartX(temp);

				temp = sprite->getPhysicalProperties()->getFinalY();
				sprite->getPhysicalProperties()->setFinalY(sprite->getPhysicalProperties()->getStartY());
				sprite->getPhysicalProperties()->setStartY(temp);

				if ((sprite->getCurrentState() == L"WALK_RIGHT")) {
					sprite->setCurrentState(L"IDLE_RIGHT");
				}
				else if ((sprite->getCurrentState() == L"WALK_LEFT")) {
					sprite->setCurrentState(L"IDLE_LEFT");
				}
				else if ((sprite->getCurrentState() == L"WALK_FRONT")) {
					sprite->setCurrentState(L"IDLE_FRONT");
				}
				else if ((sprite->getCurrentState() == L"WALK_BACK")) {
					sprite->setCurrentState(L"IDLE_BACK");
				}
				return;
			}
		}
		selectedPathfindingCell.setCenterX(getColumnCenterX(node.column));
		selectedPathfindingCell.setCenterY(getRowCenterY(node.row));
		int gridCellWidth = this->getGridWidth();
		int gridCellHeight = this->getGridHeight();
		float diffX = getColumnCenterX(node.column) - sprite->getBoundingVolume()->getCenterX();
		float diffY = getRowCenterY(node.row) - sprite->getBoundingVolume()->getCenterY();
		float distance = sqrt((diffX * diffX) + (diffY * diffY));
		float unitX = diffX/distance;
		float unitY = diffY/distance;
		float vX = unitX * MAX_WALK_SPEED;
		float vY = unitY * MAX_WALK_SPEED;
		/*AnimatedSprite *player = game->getGSM()->getSpriteManager()->getPlayer();
		if (sprite->getSpriteType()->getSpriteTypeID() == player->getSpriteType()->getSpriteTypeID()) {
			if (player->getCurrentVelocity() < PLAYER_MAX_WALK_SPEED) {
				vX = unitX * player->getCurrentVelocity();
				vY = unitY * player->getCurrentVelocity();
			}
			else {
				player->setCurrentVelocity(PLAYER_MAX_WALK_SPEED - 1);
			}
		}*/

		sprite->getPhysicalProperties()->setVelocity(vX, vY);
		if(vX > 0 && abs(vX) > abs(vY))
			sprite->setCurrentState(L"WALK_RIGHT");
		else if (vX < 0 && abs(vX) > abs(vY))
			sprite->setCurrentState(L"WALK_LEFT");
		else if (vY > 0 && abs(vX) < abs(vY))
			sprite->setCurrentState(L"WALK_BACK");
		else if (vY < 0 && abs(vX) < abs(vY))
			sprite->setCurrentState(L"WALK_FRONT");
	}
	else
	{
		
		
		int temp = sprite->getPhysicalProperties()->getFinalX();
		sprite->getPhysicalProperties()->setFinalX(sprite->getPhysicalProperties()->getStartX());
		sprite->getPhysicalProperties()->setStartX(temp);

		temp = sprite->getPhysicalProperties()->getFinalY();
		sprite->getPhysicalProperties()->setFinalY(sprite->getPhysicalProperties()->getStartY());
		sprite->getPhysicalProperties()->setStartY(temp);

		if ((sprite->getCurrentState() == L"WALK_RIGHT")) {
			sprite->setCurrentState(L"IDLE_RIGHT");
		}
		else if ((sprite->getCurrentState() == L"WALK_LEFT")) {
			sprite->setCurrentState(L"IDLE_LEFT");
		}
		else if ((sprite->getCurrentState() == L"WALK_FRONT")) {
			sprite->setCurrentState(L"IDLE_FRONT");
		}
		else if ((sprite->getCurrentState() == L"WALK_BACK")) {
			sprite->setCurrentState(L"IDLE_BACK");
		}

	}	
}

bool OrthographicGridPathfinder::hasReachedNode(AnimatedSprite *sprite, PathNode destination)
{
	AABB *spriteAABB = sprite->getBoundingVolume();
	float xDiff = fabs(spriteAABB->getCenterX() - getColumnCenterX(destination.column));
	float yDiff = fabs(spriteAABB->getCenterY() - getRowCenterY(destination.row));
	bool xReached = xDiff < GRID_EPSILON * game->getClock()->getDeltaTime();
	bool yReached = yDiff < GRID_EPSILON * game->getClock()->getDeltaTime();
	return xReached && yReached;
}

float OrthographicGridPathfinder::getColumnCenterX(int column)
{
	int gridWidth = getGridWidth();
	float centerX = (gridWidth * column) + (gridWidth/2.0f);
	return centerX;
}

float OrthographicGridPathfinder::getRowCenterY(int row)
{
	int gridHeight = getGridHeight();
	float centerY = (gridHeight * row) + (gridHeight/2.0f);
	return centerY;
}

void OrthographicGridPathfinder::mapPath(AnimatedSprite *sprite, float worldX, float worldY)
{
	list<PathNode> *path = sprite->getCurrentPathToFollow();
	buildPath(path, sprite->getBoundingVolume()->getCenterX(), sprite->getBoundingVolume()->getCenterY(), worldX, worldY);
	sprite->resetCurrentPathNode();
}

int OrthographicGridPathfinder::getGridWidth()
{
	GameStateManager *gsm = game->getGSM();
	World *world = gsm->getWorld();
	return world->getWorldWidth()/numColumns;
}

int OrthographicGridPathfinder::getGridHeight()
{
	GameStateManager *gsm = game->getGSM();
	World *world = gsm->getWorld();
	return world->getWorldHeight()/numRows;
}


void OrthographicGridPathfinder::initGrid()
{
	pathGrid.clear();
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numColumns; j++)
		{
			int centerX = (int)getColumnCenterX(j);
			int centerY = (int)getRowCenterY(i);
			pathGrid.push_back(!(game->getGSM()->getWorld()->isInsideCollidableTile(centerX, centerY)));
		}
	}
}

bool OrthographicGridPathfinder::isWalkable(int col, int row)
{
	int index = getGridIndex(col, row);
	return pathGrid[index];
}

int OrthographicGridPathfinder::getGridIndex(int col, int row)
{
	int index = col + (row * numColumns);
	return index;
}

void OrthographicGridPathfinder::moveSelectedPathfindingCell(int worldX, int worldY)
{
	int col = worldX/getGridWidth();
	int row = worldY/getGridHeight();
	selectedPathfindingCell.setCenterX(getColumnCenterX(col));
	selectedPathfindingCell.setCenterY((float)getRowCenterY(row));
}