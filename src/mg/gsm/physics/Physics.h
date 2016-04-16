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
#include "mg\gsm\sprite\AnimatedSprite.h"

class Physics
{
private:
// USED FOR TESTING PHYSICS BY TURNING IT ON AND OFF IN VARIOUS WAYS
	bool activated;
	bool activatedForSingleUpdate;

	//// additional switches for testing
	bool playerActivated;
	bool botActivated;
	bool strafing;
	bool punching;

public:

	// CONSTRUCDT/DESTRUCTOR
	Physics() {}
	~Physics() {}

	// INLINED GET/SET METHODS
	bool				isActivated()					{ return activated;					}
	bool				isActivatedForSingleUpdate()	{ return activatedForSingleUpdate;	}
	bool isStrafing() { return strafing; }
	bool isPunching() { return punching; }

	// PUBLIC METHODS DEFINED INSIDE Physics.cpp - YOU ARE WELCOME TO ADD MORE OR CHANGE WHAT YOU LIKE
	void togglePhysics() { activated = !activated; }
	void activateForSingleUpdate() { activatedForSingleUpdate = true; }
	void punch(AnimatedSprite *sprite, bool player, bool safety);

	// ALL SUBSYTEMS HAVE THESE
	void startUp();
	void shutDown();
	void update();

	//// --- physics helper functions
	bool doesSpriteGoOutWorldThisFrame(AnimatedSprite *sprite);

	//// --- player physics update enable
	void togglePlayerPhysics() { playerActivated = !playerActivated; }
	void toggleBotPhysics() { botActivated = !botActivated; }
	void toggleStrafe() { strafing = !strafing; }

};