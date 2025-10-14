#pragma once

class Game;
class GameObject;

class GameController
{
public:
	static const int MAX_PLAYERS = 2;
	
	GameController(Game* game, PlayerId_t player1Id, PlayerId_t player2Id);

	void SetClientReady(const PlayerId_t playerId);
	void MovePlayer(GameObject* playerCharacter);

private:
	Game* _game;

	unordered_map<PlayerId_t, int> _playerIdToPlayerIdxMap;

	// game state 변경을 동기화하기 위해 사용
	bool _isPlayerReadyToGoNextState[MAX_PLAYERS] = { false, false };
	int _numPlayersReadyCount{ 0 };
};
