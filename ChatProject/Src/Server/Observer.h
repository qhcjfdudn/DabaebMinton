#pragma once

enum class ObserverEvent
{
	EngineOn, EngineOff
};

class Observer
{
public:
	void notify(ObserverEvent oe);
};

