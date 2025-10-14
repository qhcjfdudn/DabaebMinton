#pragma once

#include "LinkingContext.h"
#include "Level.h"

#include "GameConfig.h"

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
	Game(ClientProxy* player1, ClientProxy* player2);
	~Game();

	ClientProxy* GetPlayerProxy(int idx) const { return _player[idx]; }

	GamePlayState GetGamePlayState() const { return _gamePlayState; }

	Level& GetLevel() { return _level; }

	bool HasElapsedReplicationInterval();
	void SetNextReplicationTimeFromNow();

	void SetClientReady(const PlayerId_t playerId);

	void StartGame();
	void EndGame();

	void MovePlayer(GameObject* playerCharacter);

	void SendOutgoingPacket();

	atomic<bool> IsPendingReplicationUpdate{ false };

private:
	// For Networking
	ClientProxy* _player[2];

	Level _level;
	
	GamePlayState _gamePlayState;

	unordered_map<PlayerId_t, int> _playerIdToPlayerIdxMap;

	// game state 변경을 동기화하기 위해 사용
	bool _isPlayerReadyToGoNextState[GameConfig::MAX_PLAYERS] = {};
	int _numPlayersReadyCount{ 0 };
	
	system_clock::time_point _nextReplicationUpdateTime;
};
