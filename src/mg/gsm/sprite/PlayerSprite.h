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

enum PlayerDirection
{
	ENUM_PLAYER_DIRECTION_UP,
	ENUM_PLAYER_DIRECTION_DOWN,
	ENUM_PLAYER_DIRECTION_RIGHT,
	ENUM_PLAYER_DIRECTION_LEFT
};

//// game-mechanic state for player sprites 
static const string MG_PLAYER_STATE_IDLE = "PLAYER_IDLE";
static const string MG_PLAYER_STATE_DEAD = "PLAYER_DEAD";
static const string MG_PLAYER_STATE_DYING = "PLAYER_DYING";
static const string MG_PLAYER_STATE_MOVING = "PLAYER_MOVING";
static const string MG_PLAYER_STATE_SHOOTING = "PLAYER_SHOOTING";

//// all animation states for player sprite -> look up Rebelle.xml file - total 28 animation types
static const string MG_PLAYER_ANIMATION_STATE_WALK_BACK = "WALK_BACK";	// 1

static const string MG_PLAYER_ANIMATION_STATE_DAMAGE_BACK = "DAMAGE_BACK";	// 2

static const string MG_PLAYER_ANIMATION_STATE_IDLE_BACK = "IDLE_BACK";	// 3
static const string MG_PLAYER_ANIMATION_STATE_IDLE_FRONT = "IDLE_FRONT";	// 4
static const string MG_PLAYER_ANIMATION_STATE_IDLE_LEFT = "IDLE_LEFT";	// 5
static const string MG_PLAYER_ANIMATION_STATE_IDLE_RIGHT = "IDLE_RIGHT";	// 6

static const string MG_PLAYER_ANIMATION_STATE_JUMP_BACK = "JUMP_BACK";	// 7
static const string MG_PLAYER_ANIMATION_STATE_JUMP_FRONT = "JUMP_FRONT";	// 8
static const string MG_PLAYER_ANIMATION_STATE_JUMP_LEFT = "JUMP_LEFT";	// 9
static const string MG_PLAYER_ANIMATION_STATE_JUMP_RIGHT = "JUMP_RIGHT";	// 10

static const string MG_PLAYER_ANIMATION_STATE_PUNCH_BACK = "PUNCH_BACK";	// 11
static const string MG_PLAYER_ANIMATION_STATE_PUNCH_LEFT = "PUNCH_LEFT";	// 12
static const string MG_PLAYER_ANIMATION_STATE_PUNCH_RIGHT = "PUNCH_RIGHT";	// 13
static const string MG_PLAYER_ANIMATION_STATE_PUNCH_FRONT = "PUNCH_FRONT";	// 14

static const string MG_PLAYER_ANIMATION_STATE_WALK_FRONT = "WALK_FRONT";	// 15
static const string MG_PLAYER_ANIMATION_STATE_WALK_LEFT = "WALK_LEFT";	// 16
static const string MG_PLAYER_ANIMATION_STATE_WALK_RIGHT = "WALK_RIGHT";	// 17

static const string MG_PLAYER_ANIMATION_STATE_SHOOT_BACK = "SHOOT_BACK";	// 18
static const string MG_PLAYER_ANIMATION_STATE_SHOOT_FRONT = "SHOOT_FRONT";	// 19
static const string MG_PLAYER_ANIMATION_STATE_SHOOT_LEFT = "SHHOT_LEFT";	// 20
static const string MG_PLAYER_ANIMATION_STATE_SHOOT_RIGHT = "SHOOT_RIGHT";	// 21

static const string MG_PLAYER_ANIMATION_STATE_STRAFE_BACK = "STRAFE_BACK";	// 22
static const string MG_PLAYER_ANIMATION_STATE_STRAFE_FRONT = "STRAFE_FRONT";	// 23
static const string MG_PLAYER_ANIMATION_STATE_STRAFE_LEFT = "STRAFE_LEFT";	// 24
static const string MG_PLAYER_ANIMATION_STATE_STRAFE_RIGHT = "STRAFE_RIGHT";	// 25

static const string MG_PLAYER_ANIMATION_STATE_DAMAGE_FRONT = "DAMAGE_FRONT";	// 26
static const string MG_PLAYER_ANIMATION_STATE_DAMAGE_LEFT = "DAMAGE_LEFT";	// 27
static const string MG_PLAYER_ANIMATION_STATE_DAMAGE_RIGHT = "DAMAGE_RIGHT";	// 28

static const int MG_NO_PLAYER_INPUT = -1;		//// playerInputStorage == MG_NO_PLAYER_INPUT when no player input from keys

static const float MG_DEFAULT_PLAYER_SPEED = 10.0f;

class PlayerSprite : public AnimatedSprite
{
private:
	PlayerState playerState;
	PlayerDirection playerDirection;
	unsigned int playerInputStorage;

	int health;
	int attack;
	int defense;
	int speed;
	int maxhealth;

public:
	PlayerSprite() {}
	~PlayerSprite() {}

	//// --- player state is different from AnimatedSprite::currentState
	PlayerState getPlayerState() { return playerState; }
	wstring getWstringForPlayerState(PlayerState state);
	PlayerState getPlayerStateForString(string state);
	
	void setHealth(int newhealth) { health = newhealth; }
	void setAttack(int newattack) { attack = newattack; }
	void setDefense(int newdefense) { defense = newdefense; }
	void setSpeed(int newspeed) { speed = newspeed; }
	void setMaxhealth(int newhealth) { maxhealth = newhealth; }

	int getHealth() { return health; }
	int getAttack() { return attack; }
	int getDefense() { return defense; }
	int getSpeed() { return health; }
	int getMaxhealth() { return maxhealth; }

	void setPlayerState(PlayerState state)
	{
		playerState = state;
	}
	
	unsigned int getPlayerInputStorage() { return playerInputStorage; }
	void setPlayerInputStorage(int initInputValue) {
		playerInputStorage = initInputValue;
	}
	
	PlayerDirection getPlayerDirection() { return playerDirection; }
	void setPlayerDirection(PlayerDirection initPD) {
		playerDirection = initPD;
	}
};