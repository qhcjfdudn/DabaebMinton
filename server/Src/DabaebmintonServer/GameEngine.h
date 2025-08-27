#pragma once

#include "Observer.h"

class GameEngine
{
public:
	static GameEngine& GetInstance();

	void TurnOff();

	bool isRunning = true;

private:
	Observer observer;
};

