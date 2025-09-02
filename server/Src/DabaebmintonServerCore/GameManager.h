#pragma once

class Game;
class ClientInfo;

class GameManager
{
public:
	static GameManager& GetInstance();

	bool CreateGame(unsigned long long gameKey, ClientInfo* player1, ClientInfo* player2);
	void RemoveGame(unsigned long long gameKey);

	Game* FindGame(ULONG_PTR completionKey);

	std::vector<std::shared_ptr<Game> > _games;
	std::mutex _gamesMutex;

	std::queue<Game*> _pendingReplicationQueue;
	std::mutex _pendingReplicationMutex;
	std::condition_variable _replicationCv;


private:
	std::unordered_map<unsigned long long, size_t> _gameKeyToGameIdxMap;
	std::unordered_map<ULONG_PTR, size_t> _completionKeyToGameIdxMap;
};