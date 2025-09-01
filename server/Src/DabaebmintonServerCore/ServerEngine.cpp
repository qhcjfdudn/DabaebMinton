#include "ServerPCH.h"
#include "ServerEngine.h"

ServerEngine& ServerEngine::GetInstance() {
	static ServerEngine instance;
	return instance;
}

void ServerEngine::TurnOff()
{
	isRunning = false;
	observer.notify(ObserverEvent::EngineOff);
}