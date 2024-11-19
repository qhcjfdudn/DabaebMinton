#include "ServerPCH.h"
#include "Engine.h"

Engine& Engine::GetInstance() {
	static Engine instance;
	return instance;
}

Engine::Engine() : isRunning(true) {}