#pragma once

#include "LinkingContext.h"
#include "Level.h"

#include "GameController.h"

class ClientInfo;

enum class GamePlayState {
	Initializing,
	Playing,
	End
};

enum class GameReplicationState {
	None,
	Pending
};

class Game
{
public:
	Game(ClientInfo* player1, ClientInfo* player2);

	bool HasElapsedReplicationInterval();
	void SetNextReplicationTimeFromNow();

	void SendPacket();
	void SendPacket(ClientInfo* client);

	Level _level;
	
	GameController gameController;

	// For Networking
	unsigned long long _gameKey{ 0 };
	ClientInfo* p_player1;
	ClientInfo* p_player2;
	LinkingContext _linkingContext;

	system_clock::time_point _nextReplicationUpdatedTime;
	atomic<GameReplicationState> _replicationState{ GameReplicationState::None };

private:
	GamePlayState _gamePlayState;
};
