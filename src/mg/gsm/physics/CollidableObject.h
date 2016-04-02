#pragma once
#include "mg_VS\stdafx.h"
#include "mg\gsm\physics\PhysicalProperties.h"

class CollidableObject
{
protected:
	bool currentlyCollidable;
	PhysicalProperties pp;

public:
	CollidableObject()	{}
	virtual ~CollidableObject()	{}

	// INLINED METHODS
	bool				isCurrentlyCollidable()		{ return currentlyCollidable;	}
	PhysicalProperties* getPhysicalProperties()		{ return &pp;					}

	void setCurrentlyCollidable(bool initCurrentlyCollidable)
	{	currentlyCollidable = initCurrentlyCollidable; }
};