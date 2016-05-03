#pragma once
#include "mg_VS\stdafx.h"

class Upgrade
{

private:

	wstring type;
	int time;
	bool activated;

public:

	wstring getType() { return type; }
	int getTime() { return time; }
	bool getActivated() { return activated; }

	void setType(wstring newtype) { type = newtype; }
	void setTime(int newtime) { time = newtime; }
	void setActivated(bool newactivated) { activated = newactivated; }
};