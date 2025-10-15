#pragma once

#include "GameConfig.h"
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
	Game(ClientProxy* (&players)[GameConfig::MAX_PLAYERS]);
	~Game();

	ClientProxy* GetPlayerProxy(int idx) const { return _player[idx]; }

	GamePlayState GetGamePlayState() const { return _gamePlayState; }

	Level& GetLevel() { return _level; }

	void StartGame();
	void EndGame();

	bool HasElapsedReplicationInterval() const;
	void SetNextReplicationTimeFromNow();
	
	void SetNextStepPhysicsTime(const steady_clock::time_point& time);

	bool StepPhysicsIfPossible();
	bool HasElapsedStepPhysicsInterval(const steady_clock::time_point& time) const;
	void StepPhysics(const steady_clock::time_point& curTime);
	void fetchNextStepPhysicsTime();

	void SendOutgoingPacket();
	
	// RPC
	void SetClientReady(const PlayerId_t playerId);
	void MovePlayer(GameObject* playerCharacter);

	atomic<bool> IsPendingReplicationUpdate{ false };

private:
	// For Networking
	ClientProxy* _player[GameConfig::MAX_PLAYERS];

	Level _level;
	
	GamePlayState _gamePlayState;

	unordered_map<PlayerId_t, int> _playerIdToPlayerIdxMap;

	// game state 변경을 동기화하기 위해 사용
	bool _isPlayerReadyToGoNextState[GameConfig::MAX_PLAYERS] = {};
	int _numPlayersReadyCount{ 0 };
	
	system_clock::time_point _nextReplicationUpdateTime;
	steady_clock::time_point _nextStepPhysicsPeriod;
	steady_clock::time_point _lastRealStepPhysicsTime;
};
