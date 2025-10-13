#pragma once

#include "SessionToken.h"

class Game;
class ClientProxy;

class GameManager
{
public:
	static GameManager& GetInstance();

	Game* CreateGame(const SessionToken (&sessions)[2]);
	bool RemoveGame(ClientProxy* clientProxy);
	Game* FindGame(const ClientProxy* clientProxy);

	int RemoveAllGames();

	std::vector<std::shared_ptr<Game> > _games;
	std::mutex _gamesMutex;

	std::queue<Game*> _pendingOutgoingPacketProcessQueue;
	std::mutex _pendingOutgoingPacketProcessMutex;
	std::condition_variable _pendingOutgoingPacketProcessCv;

private:
	std::unordered_map<const ClientProxy*, size_t> _clientProxyToGameIdxMap;
};