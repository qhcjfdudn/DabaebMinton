#include "ServerPCH.h"
#include "DeveloperCommandFunctor.h"

#include <conio.h>

#include "ServerEngine.h"

DeveloperCommandFunctor::DeveloperCommandFunctor(vector<Level>& levels)
	: _levels(levels)
{
}

void DeveloperCommandFunctor::operator()()
{
	auto& gameEngine = ServerEngine::GetInstance();
	string cmd = "";
	while (gameEngine.isRunning.load(std::memory_order_acquire))
	{
		if (_kbhit())
		{
			char c = _getch();
			if (c == 13 /* CR */)
			{
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
				cmd = "";
			}
			else {
				cmd += c;
			}
		}
	}
}