#include "mg\gsm\sprite\PlayerSprite.h"

wstring PlayerSprite::getWstringForPlayerState(PlayerState state)
{
	if (state == ENUM_PLAYER_IDLE) return wstring(MG_PLAYER_STATE_IDLE.begin(), MG_PLAYER_STATE_IDLE.end());
	else if (state == ENUM_PLAYER_DYING) return wstring(MG_PLAYER_STATE_DYING.begin(), MG_PLAYER_STATE_DYING.end());
	else if (state == ENUM_PLAYER_DEAD) return wstring(MG_PLAYER_STATE_DEAD.begin(), MG_PLAYER_STATE_DEAD.end());
	else if (state == ENUM_PLAYER_MOVING) return wstring(MG_PLAYER_STATE_MOVING.begin(), MG_PLAYER_STATE_MOVING.end());
	else if (state == ENUM_PLAYER_SHOOTING) return wstring(MG_PLAYER_STATE_SHOOTING.begin(), MG_PLAYER_STATE_SHOOTING.end());
	else return L"";
}

PlayerState PlayerSprite::getPlayerStateForString(string state)
{
	if (state.compare(MG_PLAYER_STATE_IDLE) == 0) return PlayerState::ENUM_PLAYER_IDLE;
	else if (state.compare(MG_PLAYER_STATE_DEAD) == 0) return PlayerState::ENUM_PLAYER_DEAD;
	else if (state.compare(MG_PLAYER_STATE_DYING) == 0) return PlayerState::ENUM_PLAYER_DYING;
	else if (state.compare(MG_PLAYER_STATE_MOVING) == 0) return PlayerState::ENUM_PLAYER_MOVING;
	else if (state.compare(MG_PLAYER_STATE_SHOOTING) == 0) return PlayerState::ENUM_PLAYER_SHOOTING;
	else return PlayerState::ENUM_PLAYER_NO_STATE;
}
