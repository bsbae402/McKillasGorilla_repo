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
	bool activated = true;
	bool activatedForSingleUpdate = true;

	//// additional switches for testing
	bool playerActivated = true;
	bool botActivated = true;
	bool strafing;
	bool punching;
	bool gameover;

	int exit = 0;
	bool punched;

	int countdown = 100;
	

public:

	// CONSTRUCDT/DESTRUCTOR
	Physics() {}
	~Physics() {}

	// INLINED GET/SET METHODS
	bool				isActivated()					{ return activated;					}
	bool				isActivatedForSingleUpdate()	{ return activatedForSingleUpdate;	}
	bool isStrafing() { return strafing; }
	bool isPunching() { return punching; }
	bool isPlayerActivated() { return playerActivated; }
	bool setPlayerActivated(bool active) { return playerActivated; }
	void setGameOver(bool newgameover) { gameover = newgameover; }
	bool getGameOver() { return gameover; }
	

	// PUBLIC METHODS DEFINED INSIDE Physics.cpp - YOU ARE WELCOME TO ADD MORE OR CHANGE WHAT YOU LIKE
	void togglePhysics() { activated = !activated; }
	void activateForSingleUpdate() { activatedForSingleUpdate = true; }
	void punch(AnimatedSprite *sprite, bool player, bool safety);
	void CheckPunchShoot(AnimatedSprite *playersprite);
	void CheckDying(AnimatedSprite *playersprite);
	void GameOverCountDown();

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