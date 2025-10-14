#pragma once

#include "LinkingContext.h"
#include "Level.h"

#include "GameController.h"

class ClientProxy;

enum class GamePlayState {
	Initializing,
	Playing,
	End,
	Size
};

enum class GameReplicationState {
	None,
	Pending
};

class Game
{
public:
	Game(ClientProxy* player1, ClientProxy* player2);
	~Game();

	GameController& GetGameController() { return _gameController; }

	GamePlayState GetGamePlayState() const { return _gamePlayState; }

	void StartGame();

	bool HasElapsedReplicationInterval();
	void SetNextReplicationTimeFromNow();

	Level _level;

	// For Networking
	ClientProxy* p_player1;
	ClientProxy* p_player2;

	system_clock::time_point _nextReplicationUpdatedTime;
	atomic<GameReplicationState> _replicationState{ GameReplicationState::None };

private:
	GamePlayState _gamePlayState;
	GameController _gameController;
};
