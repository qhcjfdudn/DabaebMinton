#include "ServerPCH.h"
#include "GameManager.h"

#include "Game.h"
#include "ClientInfo.h"

GameManager& GameManager::GetInstance() {
	static GameManager instance;
	return instance;
}

bool GameManager::CreateGame(unsigned long long gameKey, ClientInfo* player1, ClientInfo* player2)
{
	if (_gameKeyToGameIdxMap.find(gameKey) != _gameKeyToGameIdxMap.end())
	{
		cout << "[GameManager::CreateGame] A same gameKey already exists. gameKey: " << gameKey << endl;
		return false;
	}

	// 전달 받은 sockaddr_in 정보로 Client를 만들어서 Game의 생성자 인자로 전달해줄 것.

	shared_ptr<Game> game = make_shared<Game>(player1, player2);
	Game* p_game = game.get();

	_gamesMutex.lock();
	size_t idx = _games.size();
	_games.push_back(game);
	_gamesMutex.unlock();

	_gameKeyToGameIdxMap.emplace(gameKey, idx);
	_completionKeyToGameIdxMap.emplace(player1->_completionKey, idx);
	_completionKeyToGameIdxMap.emplace(player2->_completionKey, idx);

    return true;
}

void GameManager::RemoveGame(unsigned long long gameKey)
{
	if (_gameKeyToGameIdxMap.find(gameKey) == _gameKeyToGameIdxMap.end())
	{
		cout << "[GameManager::RemoveGame] A gameKey does not exist. gameKey: " << gameKey << endl;
		return;
	}

	size_t gameIdx = _gameKeyToGameIdxMap[gameKey];
	
	_gamesMutex.lock();
	shared_ptr<Game> game = _games[gameIdx];

	_gameKeyToGameIdxMap.erase(gameKey);
	_completionKeyToGameIdxMap.erase(game->p_player1->_completionKey);
	_completionKeyToGameIdxMap.erase(game->p_player2->_completionKey);

	swap(_games[gameIdx], _games[_games.size() - 1]);
	_games.pop_back();
	_gamesMutex.unlock();
}

Game* GameManager::FindGame(ULONG_PTR completionKey)
{
	if (_completionKeyToGameIdxMap.find(completionKey) == _completionKeyToGameIdxMap.end())
	{
		cout << "[GameManager::FindGame] No game is found to use this completionKey: " << completionKey << endl;
		return nullptr;
	}

	std::lock_guard lk(_gamesMutex);
	return _games[_completionKeyToGameIdxMap[completionKey]].get();
}
