#include "ServerPCH.h"
#include "GameEngine.h"

GameEngine& GameEngine::GetInstance() {
	static GameEngine instance;
	return instance;
}

void GameEngine::TurnOff()
{
	isRunning = false;
	observer.notify(ObserverEvent::EngineOff);
}