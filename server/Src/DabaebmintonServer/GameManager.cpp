#include "ServerPCH.h"
#include "GameManager.h"

#include "Game.h"

bool GameManager::CreateGame(unsigned long long gameKey, const sockaddr_in& player1, const sockaddr_in& player2)
{
	if (_gameKeyToGameMap.find(gameKey) != _gameKeyToGameMap.end())
	{
		cout << "[GameManager::CreateGame] A same gameKey already exists. gameKey: " << gameKey << endl;
		return false;
	}

	// 전달 받은 sockaddr_in 정보로 Client를 만들어서 Game의 생성자 인자로 전달해줄 것.

	_gameKeyToGameMap.emplace(gameKey, make_shared<Game>());

    return true;
}

void GameManager::RemoveGame(unsigned long long gameKey)
{
	if (_gameKeyToGameMap.find(gameKey) == _gameKeyToGameMap.end())
	{
		cout << "[GameManager::RemoveGame] A gameKey does not exist. gameKey: " << gameKey << endl;
		return;
	}

	_gameKeyToGameMap.erase(gameKey);
}

Game* GameManager::FindGame(ULONG_PTR completionKey)
{
	if (_completionKeyToGameMap.find(completionKey) == _completionKeyToGameMap.end())
	{
		cout << "[GameManager::FindGame] No game is found to use this completionKey: " << completionKey << endl;
		return nullptr;
	}

	return _completionKeyToGameMap[completionKey].get();
}
