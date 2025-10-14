#include "ServerPCH.h"
#include "GameManager.h"

#include "NetworkManagerServer.h"
#include "Game.h"
#include "ClientProxy.h"

using namespace GameConfig;

GameManager& GameManager::GetInstance() {
	static GameManager instance;
	return instance;
}

Game* GameManager::CreateGame(const SessionToken(&sessions)[GameConfig::MAX_PLAYERS])
{
	Game* ret = nullptr;

	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	ClientProxy* clientProxies[MAX_PLAYERS];

	auto& mp = _clientProxyToGameIdxMap;
	_gamesMutex.lock();
	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		clientProxies[i] = networkManagerServer.CreateClientProxy(sessions[i]);

		if (mp.find(clientProxies[i]) != mp.end())
		{
			ret = _games[mp[clientProxies[i]]].get();
			_gamesMutex.unlock();

			spdlog::info("[GameManager::CreateGame] A game already exists.");

			return ret;
		}
	}

	shared_ptr<Game> game = make_shared<Game>(clientProxies);
	ret = game.get();

	size_t gameIdx = _games.size();
	_games.push_back(game);
	_gamesMutex.unlock();

	for (int i = 0; i < MAX_PLAYERS; ++i)
	{
		_clientProxyToGameIdxMap.emplace(clientProxies[i], gameIdx);
	}

	return ret;
}

bool GameManager::RemoveGame(ClientProxy* clientProxy)
{
	_gamesMutex.lock();
	if (_clientProxyToGameIdxMap.find(clientProxy) == _clientProxyToGameIdxMap.end())
	{
		_gamesMutex.unlock();

		spdlog::warn("[GameManager::RemoveGame] A game does not exist.");

		return false;
	}

	size_t gameIdx = _clientProxyToGameIdxMap[clientProxy];

	shared_ptr<Game> game = _games[gameIdx];
	for (int playerIdx = 0; playerIdx < GameConfig::MAX_PLAYERS; ++playerIdx)
	{
		_clientProxyToGameIdxMap.erase(game->GetPlayerProxy(playerIdx));
	}

	swap(_games[gameIdx], _games[_games.size() - 1]);
	_games.pop_back();
	
	_gamesMutex.unlock();

	return true;
}

Game* GameManager::FindGame(const ClientProxy* clientProxy)
{
	_gamesMutex.lock();
	if (_clientProxyToGameIdxMap.find(clientProxy) == _clientProxyToGameIdxMap.end())
	{
		_gamesMutex.unlock();

		cout << "[GameManager::FindGame] No game is found to use this clientProxy: " << clientProxy << endl;
		return nullptr;
	}
	
	Game* game = _games[_clientProxyToGameIdxMap[clientProxy]].get();
	
	_gamesMutex.unlock();

	return game;
}

int GameManager::RemoveAllGames()
{
	_gamesMutex.lock();
	
	int ret = static_cast<int>(_games.size());
	_games.clear();
	_clientProxyToGameIdxMap.clear();

	_gamesMutex.unlock();

	return ret;
}
