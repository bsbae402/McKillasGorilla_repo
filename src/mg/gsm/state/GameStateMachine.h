#pragma once

class GameStateMachine
{
public:
	GameStateMachine() {}
	virtual ~GameStateMachine() {}
	virtual void update() = 0;
};