#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Constant.h"
#include "ClientInfo.h"
#include "Packet.h"
#include "GameObject.h"

Game::Game(ClientInfo* player1, ClientInfo* player2) :
	p_player1{ player1 }, p_player2{ player2 },
	_gamePlayState{ GamePlayState::Initializing },
	_level{},
	gameController{}
{
	_level.InitLevel();

	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.gameObjects)
	{
		spdlog::debug("[Game::Game] object: {}", gameObject->GetClassId());

		auto networkId = networkManager.RegisterGameObject(gameObject);		
		player1->GetReplicationManager().ReplicateCreate(networkId, gameObject->GetAllStateMask());
		player2->GetReplicationManager().ReplicateCreate(networkId, gameObject->GetAllStateMask());
	}

	SetNextReplicationTimeFromNow();
}

Game::~Game()
{
	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.gameObjects)
	{
		auto networkId = gameObject->GetNetworkId();
		networkManager.UnregisterGameObject(networkId);
		p_player1->GetReplicationManager().ReplicateDestroy(networkId);
		p_player2->GetReplicationManager().ReplicateDestroy(networkId);
	}
}

bool Game::HasElapsedReplicationInterval()
{
	return system_clock::now() >= _nextReplicationUpdatedTime;
}

void Game::SetNextReplicationTimeFromNow()
{
	std::chrono::duration<float> offset(Constant::REPLICATION_PERIOD);
	_nextReplicationUpdatedTime = system_clock::now() + std::chrono::duration_cast<system_clock::duration>(offset);
}
