#pragma once

#include "LinkingContext.h"

class Level;
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
	Level* p_level;
	
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

