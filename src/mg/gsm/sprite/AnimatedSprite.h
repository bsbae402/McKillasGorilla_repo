/*	
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	AnimatedSprite.h

	This class represents a sprite that can can
	be used to animate a game character or object.
*/

#pragma once
#include "mg_VS\stdafx.h"
#include "mg\gsm\physics\CollidableObject.h"
#include "mg\gsm\physics\PhysicalProperties.h"
#include "mg\gsm\sprite\AnimatedSpriteType.h"
#include "mg\gui\Viewport.h"
#include "mg\gsm\physics\AABB.h"
#include "mg\gsm\ai\pathfinding\GridPathfinder.h"
#include "mg\gsm\ai\pathfinding\OrthographicGridPathfinder.h"

class AnimatedSprite : public CollidableObject
{
protected:
	// SPRITE TYPE FOR THIS SPRITE. THE SPRITE TYPE IS THE ID
	// OF AN AnimatedSpriteType OBJECT AS STORED IN THE SpriteManager
	AnimatedSpriteType *spriteType;

	// TRANSPARENCY/OPACITY
	int alpha;

	// THE "current" STATE DICTATES WHICH ANIMATION SEQUENCE 
	// IS CURRENTLY IN USE, BUT IT MAP ALSO BE USED TO HELP
	// WITH OTHER GAME ACTIVITIES, LIKE PHYSICS
	wstring currentState;
	wstring previousState;

	// THE INDEX OF THE CURRENT FRAME IN THE ANIMATION SEQUENCE
	// NOTE THAT WE WILL COUNT BY 2s FOR THIS SINCE THE VECTOR
	// THAT STORES THIS DATA HAS (ID,DURATION) TUPLES
	unsigned int frameIndex;

	// USED TO ITERATE THROUGH THE CURRENT ANIMATION SEQUENCE
	unsigned int animationCounter;

	// USED TO RENDER A ROTATED SPRITE, NOT INVOLVED IN PHYSICS
	float rotationInRadians;

	// HELPS US KEEP TRACK OF WHEN TO REMOVE IT
	bool markedForRemoval;

	// BOUNDING VOLUME FOR THE ANIMATED SPRITE
	AABB *boundingVolume;


	// CURRENT PATH OF THE ANIMATED SPRITE
	list<PathNode> *currentPathToFollow;

public:
	// INLINED ACCESSOR METHODS
	int					getAlpha()				{ return alpha;					}
	wstring				getCurrentState()		{ return currentState;			}
	wstring				getPreviousState() { return previousState; }
	unsigned int		getFrameIndex()			{ return frameIndex;			}
	float				getRotationInRadians()	{ return rotationInRadians;		}
	AnimatedSpriteType*	getSpriteType()			{ return spriteType;			}
	bool				isMarkedForRemoval() { return markedForRemoval; }
	AABB*				getBoundingVolume() { return boundingVolume; }
	list<PathNode>*		getCurrentPathToFollow() { return currentPathToFollow; }
	list<PathNode>::iterator	getCurrentPathNode() { return currentPathToFollow->begin(); }
	void				advanceCurrentPathNode() { currentPathToFollow->pop_front(); }
	void				clearPath() { currentPathToFollow->clear(); }
	bool				hasReachedDestination() { return currentPathToFollow->empty(); }
	void				resetCurrentPathNode() { currentPathToFollow->begin(); }

	void setPreviousState(wstring newState) { previousState = newState; }

	void setBoundingVolume() {
		boundingVolume = new AABB();
		boundingVolume->setWidth(this->getSpriteType()->getTextureWidth());
		boundingVolume->setHeight(this->getSpriteType()->getTextureHeight());
		boundingVolume->setCenterX(pp.getX() + (this->getSpriteType()->getTextureWidth() / 2));
		boundingVolume->setCenterY(pp.getY() + (this->getSpriteType()->getTextureHeight() / 2));
	}

	void setCurrentPathToFollow() {
	this->currentPathToFollow = new list<PathNode>;
	}


	// INLINED MUTATOR METHODS
	void setAlpha(int initAlpha)
	{	alpha = initAlpha;						}
	void setRotationInRadians(float initRotation)
	{	rotationInRadians = initRotation;		}
	void setSpriteType(AnimatedSpriteType *initSpriteType)
	{	spriteType = initSpriteType;			}
	void markForRemoval()
	{
		markedForRemoval = true;
	}

	// METHODS DEFINED IN AnimatedSprite.cpp
	AnimatedSprite();
	virtual ~AnimatedSprite();
	void changeFrame();
	unsigned int getCurrentImageID();
	void setCurrentState(wstring newState);
	
	void updateSprite();
};