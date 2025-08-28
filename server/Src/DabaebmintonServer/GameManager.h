#pragma once

class Game;

class GameManager
{
public:
	bool CreateGame(unsigned long long gameKey, const sockaddr_in& player1, const sockaddr_in& player2);
	void RemoveGame(unsigned long long gameKey);

	Game* FindGame(ULONG_PTR completionKey);


private:
	unordered_map<unsigned long long, shared_ptr<Game> > _gameKeyToGameMap;
	unordered_map<ULONG_PTR, shared_ptr<Game> > _completionKeyToGameMap;
};