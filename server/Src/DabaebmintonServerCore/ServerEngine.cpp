#include "ServerPCH.h"
#include "ServerEngine.h"

ServerEngine& ServerEngine::GetInstance()
{
	static ServerEngine instance;
	return instance;
}

void ServerEngine::TurnOff()
{
	isRunning.store(false, std::memory_order_release);
	observer.notify(ObserverEvent::EngineOff);
}

ServerEngine::ServerEngine() :
	isRunning{ true }
{
}