#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "Constant.h"

Game::Game(ClientInfo* player1, ClientInfo* player2) :
	p_player1{ player1 }, p_player2{ player2 },
	_gamePlayState{ GamePlayState::Initializing },
	_level{}
{
	_level.InitLevel();


	_lastReplicationUpdatedTime = system_clock::now();
}

void Game::ReplicateLevel()
{
	auto& networkManager = NetworkManagerServer::GetInstance();

	// Level의 GameObjects로부터 모두 Replication 해서 stream을 만드는 코드를 짠다.

}

bool Game::HasElapsedReplicationInterval()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastReplicationUpdatedTime;

	return elapsedTime.count() >= Constant::PACKET_PERIOD;
}

void Game::SetLastReplicationTimeToNow()
{
	_lastReplicationUpdatedTime = system_clock::now();
}
