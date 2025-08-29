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

	vector<shared_ptr<Game> > _games;
	mutex _gamesMutex;

	queue<Game*> _pendingReplicationQueue;
	mutex _pendingReplicationMutex;
	condition_variable _replicationCv;


private:
	unordered_map<unsigned long long, size_t> _gameKeyToGameIdxMap;
	unordered_map<ULONG_PTR, size_t> _completionKeyToGameIdxMap;
};