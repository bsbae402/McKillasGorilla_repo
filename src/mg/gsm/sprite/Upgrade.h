#pragma once
#include "mg_VS\stdafx.h"

class Upgrade
{

private:

	wstring type;
	int time;
	bool activated;
	bool wasactivated;

public:

	wstring getType() { return type; }
	int getTime() { return time; }
	bool getActivated() { return activated; }
	bool getWasActivated() { return wasactivated; }

	void setType(wstring newtype) { type = newtype; }
	void setTime(int newtime) { time = newtime; }
	void setActivated(bool newactivated) { activated = newactivated; }
	void setWasActivated(bool newactivated) { wasactivated = newactivated; }
};