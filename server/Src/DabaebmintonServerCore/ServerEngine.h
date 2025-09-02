#pragma once

#include "Observer.h"

class ServerEngine
{
public:
	static ServerEngine& GetInstance();

	void TurnOff();

	atomic<bool> isRunning;

private:
	ServerEngine();

	Observer observer;
};

