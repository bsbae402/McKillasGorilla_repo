#pragma once

#include "mg\gsm\sprite\AnimatedSprite.h"

//// followings are enum of playerSprite's state, not for animation_state
enum PlayerState
{
	ENUM_PLAYER_IDLE,
	ENUM_PLAYER_DEAD,
	ENUM_PLAYER_DYING,
	ENUM_PLAYER_MOVING,
	ENUM_PLAYER_SHOOTING,
	ENUM_PLAYER_NO_STATE
};

static const string MG_PLAYER_STATE_IDLE = "PLAYER_IDLE";
static const string MG_PLAYER_STATE_DEAD = "PLAYER_DEAD";
static const string MG_PLAYER_STATE_DYING = "PLAYER_DYING";
static const string MG_PLAYER_STATE_MOVING = "PLAYER_MOVING";
static const string MG_PLAYER_STATE_SHOOTING = "PLAYER_SHOOTING";

class PlayerSprite : public AnimatedSprite
{
private:
	PlayerState playerState;

public:
	PlayerSprite() {}
	~PlayerSprite() {}

	//// --- player state is different from AnimatedSprite::currentState
	PlayerState getPlayerState() { return playerState; }
	wstring getWstringForPlayerState(PlayerState state);
	PlayerState getPlayerStateForString(string state);

	void setPlayerState(PlayerState state)
	{
		playerState = state;
	}
	
};