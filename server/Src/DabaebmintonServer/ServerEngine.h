#pragma once

#include "Observer.h"

class ServerEngine
{
public:
	static ServerEngine& GetInstance();

	void TurnOff();

	bool isRunning = true;

private:
	Observer observer;
};

