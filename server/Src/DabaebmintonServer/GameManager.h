#pragma once

class Game;

class GameManager
{
public:
	static GameManager& GetInstance();

	bool CreateGame(unsigned long long gameKey, ClientInfo* player1, ClientInfo* player2);
	void RemoveGame(unsigned long long gameKey);

	Game* FindGame(ULONG_PTR completionKey);


private:
	unordered_map<unsigned long long, shared_ptr<Game> > _gameKeyToGameMap;
	unordered_map<ULONG_PTR, shared_ptr<Game> > _completionKeyToGameMap;
};