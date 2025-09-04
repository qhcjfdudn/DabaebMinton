#pragma once

enum class ObserverEvent
{
	EngineOn, EngineOff
};

class Observer
{
public:
	static void notify(const ObserverEvent& oe);
};

