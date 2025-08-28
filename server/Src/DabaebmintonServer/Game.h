#pragma once

#include "LinkingContext.h"
#include "Level.h"

class ClientInfo;

class GameController {

};

enum class GamePlayState {
	Initializing,
	Playing,
	End
};

class Game
{
public:
	Game(ClientInfo* player1, ClientInfo* player2);

	Level _level;
	
	GameController gameController;

	// For Networking
	unsigned long long _gameKey;
	ClientInfo* p_player1;
	ClientInfo* p_player2;
	LinkingContext _linkingContext;
	system_clock::time_point _lastReplicationUpdatedTime;

private:
	GamePlayState _gamePlayState;
};

