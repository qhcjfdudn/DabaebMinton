#include "ServerPCH.h"
#include "DeveloperCommandFunctor.h"

#include "GameEngine.h"

DeveloperCommandFunctor::DeveloperCommandFunctor(vector<Level>& levels)
	: _levels(levels)
{}

void DeveloperCommandFunctor::operator()() {
	auto& gameEngine = GameEngine::GetInstance();
	string cmd;
	while (gameEngine.isRunning)
	{
		std::getline(std::cin, cmd);
		if (cmd == "r")
		{
			for (auto& level : _levels)
				level.RemoveAllGameObjects();
		}
		else if (cmd == "s")
		{
			for (auto& level : _levels)
				level.InitLevel();
		}
	}
}