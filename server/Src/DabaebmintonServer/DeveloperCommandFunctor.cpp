#include "ServerPCH.h"
#include "DeveloperCommandFunctor.h"

#include <conio.h>

#include "ServerEngine.h"
#include "GameManager.h"
#include "Game.h"

void DeveloperCommandFunctor::operator()()
{
	auto& gameEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	auto& games = gameManager._games;

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
					std::lock_guard<mutex> lk(gameManager._gamesMutex);
					for (auto& game : games)
						game->_level.RemoveAllGameObjects();
				}
				else if (cmd == "s")
				{
					// 디버깅 목적으로 필요한 코드 추가 가능
				}
				cmd = "";
			}
			else {
				cmd += c;
			}
		}
	}
}