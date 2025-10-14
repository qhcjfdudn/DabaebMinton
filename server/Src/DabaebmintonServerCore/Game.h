#pragma once

#include "LinkingContext.h"
#include "Level.h"

class ClientProxy;

enum class GamePlayState {
	Initializing,
	Playing,
	End,
	Size
};

class Game
{
public:
	static const int MAX_PLAYERS = 2;

	Game(ClientProxy* player1, ClientProxy* player2);
	~Game();

	GamePlayState GetGamePlayState() const { return _gamePlayState; }


	bool HasElapsedReplicationInterval();
	void SetNextReplicationTimeFromNow();

	void SetClientReady(const PlayerId_t playerId);

	void StartGame();

	void MovePlayer(GameObject* playerCharacter);

	Level _level;

	// For Networking
	ClientProxy* p_player1;
	ClientProxy* p_player2;

	system_clock::time_point _nextReplicationUpdateTime;
	atomic<bool> IsPendingReplicationUpdate{ false };

private:
	GamePlayState _gamePlayState;

	unordered_map<PlayerId_t, int> _playerIdToPlayerIdxMap;

	// game state 변경을 동기화하기 위해 사용
	bool _isPlayerReadyToGoNextState[MAX_PLAYERS] = { false, false };
	int _numPlayersReadyCount{ 0 };
};
