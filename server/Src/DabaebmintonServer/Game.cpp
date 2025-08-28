#include "ServerPCH.h"
#include "Game.h"

Game::Game(ClientInfo* player1, ClientInfo* player2) :
	p_player1{ player1 }, p_player2{ player2 },
	_gamePlayState{ GamePlayState::Initializing },
	_level{}
{



	//_level.InitLevel();


	_lastReplicationUpdatedTime = system_clock::now();
}
