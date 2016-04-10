/*
	Author: Richard McKenna
			Stony Brook University
			Computer Science Department

	AnimatedSprite.cpp

	See AnimatedSprite.h for a class description.
*/

#include "mg_VS\stdafx.h"
#include "mg\game\Game.h"
#include "mg\gsm\physics\PhysicalProperties.h"
#include "mg\gsm\sprite\AnimatedSprite.h"
#include "mg\gsm\sprite\AnimatedSpriteType.h"

#include "mg\gsm\state\GameStateManager.h"

/*
	AnimatedSprite - Default constructor, just sets everything to 0.
*/
AnimatedSprite::AnimatedSprite()  
{
	spriteType = 0;
	frameIndex = 0;
	animationCounter = 0;
	pp.setVelocity(0.0f, 0.0f);
	pp.setAccelerationX(0.0f);
	pp.setAccelerationY(0.0f);
	pp.setPosition(0.0f, 0.0f);
	markedForRemoval = false;
}

/*
	Destructor - Nothing to clean up. We don't want to clean up the
	sprite type because it is a shared variable.
*/
AnimatedSprite::~AnimatedSprite() 
{

}

/*
	changeFrame - This method allows for the changing of an image in an 
	animation sequence for a given animation state.
*/
void AnimatedSprite::changeFrame()
{
	// RESET THE COUNTER
	animationCounter = 0;

	// REMEMBER, WE COUNT BY 2s
	frameIndex += 2;

	// GO BACK TO FIRST INDEX IF NECESSARY
	if (frameIndex == spriteType->getSequenceSize(currentState))
		frameIndex = 0;
}

/*
	getCurrentImageID - This method does the work of retrieving
	the image id that corresponds to this sprite's current state
	and frame index.
*/
unsigned int AnimatedSprite::getCurrentImageID()
{
	return spriteType->getAnimationFrameID(currentState, frameIndex);
}

/*
	setCurrentState - This method sets this sprite to the newState
	state and starts its animtion sequence from the beginning.
*/
void AnimatedSprite::setCurrentState(wstring newState) 
{
	string cs(currentState.begin(), currentState.end());
	string ns(newState.begin(), newState.end());
	if (strcmp(cs.c_str(), ns.c_str()) != 0)
	{
		// SET THE ANIMATINO STATE
		currentState = newState;

		// AND RESET OUR COUNTERS
		animationCounter = 0;
		frameIndex = 0;
	}
}

/*
	updateSprite - To be called every frame of animation. This
    method advances the animation counter appropriately per
	the animation speed. It also updates the sprite location
	per the current velocity.
*/
void AnimatedSprite::updateSprite()
{
	unsigned int duration = spriteType->getDuration(currentState, frameIndex);
	animationCounter++;

	// WE ONLY CHANGE THE ANIMATION FRAME INDEX WHEN THE
	// ANIMATION COUNTER HAS REACHED THE DURATION
	if (animationCounter >= duration)
		changeFrame();

	//// -- player sprite image doesn't need any rotation
	Game *game = Game::getSingleton();
	GameStateManager *gsm = game->getGSM();
	if (this == gsm->getSpriteManager()->getPlayer())
	{
		return;		// so just return.
	}

	///// ---- followings are dealing with the sprite rotation
	// AND NOW CALCULATE THE RADIANS
	// FOR RENDERING THIS SPRITE
	float vX = pp.getVelocityX();
	float vY = pp.getVelocityY();
	float epsilon = 0.00001f;

	// IF THE Y VELOCITY IS 0 WE HAVE TO BE CAREFUL DIVIDING
	if (fabs(vY) < epsilon)
	{
		// IF X ISN'T MOVING EITHER, LEAVE IT WHAT IT IS
		// AND PUT IT INTO AN IDLE STATE
		if (fabs(vX) > epsilon)
		{
			if (vX < 0.0f)
			{
				rotationInRadians = M_PI + (M_PI/2);
			}
			else
				rotationInRadians = 0.0f + (M_PI / 2);
		}
	}
	else
	{
		float tanTheta = vY / vX;
		rotationInRadians = atan(tanTheta) + (M_PI / 2);
		if (vX < 0.0f)
		{
			if (vY < 0.0f)
				rotationInRadians += M_PI;
			else
				rotationInRadians -= M_PI;
		}
	}
}