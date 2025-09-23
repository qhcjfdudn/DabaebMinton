#pragma once

class Game;
class ClientProxy;

class GameManager
{
public:
	static GameManager& GetInstance();

	Game* CreateGame(const string clientIps[2], const unsigned int clientPorts[2]);
	bool RemoveGame(ClientProxy * clientProxy);
	Game* FindGame(ClientProxy * clientProxy);

	int RemoveAllGames();

	std::vector<std::shared_ptr<Game> > _games;
	std::mutex _gamesMutex;

	std::queue<Game*> _pendingReplicationQueue;
	std::mutex _pendingReplicationMutex;
	std::condition_variable _replicationCv;

private:
	std::unordered_map<ClientProxy*, size_t> _clientProxyToGameIdxMap;
};