#include "ServerPCH.h"
#include "GameManager.h"

#include "NetworkManagerServer.h"
#include "Game.h"
#include "ClientInfo.h"

GameManager& GameManager::GetInstance() {
	static GameManager instance;
	return instance;
}

Game* GameManager::CreateGame(const string clientIps[2], const unsigned int clientPorts[2])
{
	Game* ret = nullptr;

	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	ClientInfo* ci1 = networkManagerServer.CreateClientInfo(clientIps[0], clientPorts[0]);
	ClientInfo* ci2 = networkManagerServer.CreateClientInfo(clientIps[1], clientPorts[1]);
	
	auto& mp = _clientInfoToGameIdxMap;

	_gamesMutex.lock();
	if (mp.find(ci1) != mp.end())
	{
		ret = _games[mp[ci1]].get();
		_gamesMutex.unlock();
		
		spdlog::info("[GameManager::CreateGame] A game already exists.");
		
		return ret;
	}

	if (mp.find(ci2) != mp.end())
	{
		ret = _games[mp[ci2]].get();
		_gamesMutex.unlock();

		spdlog::info("[GameManager::CreateGame] A game already exists.");
		
		return ret;
	}

	shared_ptr<Game> game = make_shared<Game>(ci1, ci2);
	ret = game.get();

	size_t idx = _games.size();
	_games.push_back(game);

	_clientInfoToGameIdxMap.emplace(ci1, idx);
	_clientInfoToGameIdxMap.emplace(ci2, idx);
	
	_gamesMutex.unlock();

	return ret;
}

bool GameManager::RemoveGame(ClientInfo* clientInfo)
{
	_gamesMutex.lock();
	if (_clientInfoToGameIdxMap.find(clientInfo) == _clientInfoToGameIdxMap.end())
	{
		_gamesMutex.unlock();

		spdlog::info("[GameManager::RemoveGame] A game does not exist.");

		return false;
	}

	size_t gameIdx = _clientInfoToGameIdxMap[clientInfo];

	shared_ptr<Game> game = _games[gameIdx];
	_clientInfoToGameIdxMap.erase(game->p_player1);
	_clientInfoToGameIdxMap.erase(game->p_player2);

	swap(_games[gameIdx], _games[_games.size() - 1]);
	_games.pop_back();
	
	_gamesMutex.unlock();

	return true;
}

Game* GameManager::FindGame(ClientInfo* clientInfo)
{
	_gamesMutex.lock();
	if (_clientInfoToGameIdxMap.find(clientInfo) == _clientInfoToGameIdxMap.end())
	{
		_gamesMutex.unlock();

		cout << "[GameManager::FindGame] No game is found to use this clientInfo: " << clientInfo << endl;
		return nullptr;
	}
	
	Game* game = _games[_clientInfoToGameIdxMap[clientInfo]].get();
	
	_gamesMutex.unlock();

	return game;
}

int GameManager::RemoveAllGames()
{
	_gamesMutex.lock();
	
	int ret = static_cast<int>(_games.size());
	_games.clear();
	_clientInfoToGameIdxMap.clear();

	_gamesMutex.unlock();

	return ret;
}
