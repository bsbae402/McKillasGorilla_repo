/*	
	Author: Richard McKenna

	Physics.h

	This class will be used to manage all game collisions
	and physics. This includes collision detection, and resolution.
	Each frame we will update the collided objects velocities and
	positions accordingly.
*/

#pragma once
#include "mg_VS\stdafx.h"

class Physics
{
private:
// USED FOR TESTING PHYSICS BY TURNING IT ON AND OFF IN VARIOUS WAYS
	bool activated;
	bool activatedForSingleUpdate;

public:

	// CONSTRUCDT/DESTRUCTOR
	Physics() {}
	~Physics() {}

	// INLINED GET/SET METHODS
	bool				isActivated()					{ return activated;					}
	bool				isActivatedForSingleUpdate()	{ return activatedForSingleUpdate;	}

	// PUBLIC METHODS DEFINED INSIDE Physics.cpp - YOU ARE WELCOME TO ADD MORE OR CHANGE WHAT YOU LIKE
	void togglePhysics() { activated = !activated; }
	void activateForSingleUpdate() { activatedForSingleUpdate = true; }

	// ALL SUBSYTEMS HAVE THESE
	void startUp();
	void shutDown();
	void update();
};