#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Constant.h"
#include "ClientProxy.h"
#include "Packet.h"
#include "GameObject.h"

using namespace GameConfig;

Game::Game(ClientProxy* (&players)[GameConfig::MAX_PLAYERS]) :
	_gamePlayState{ GamePlayState::Initializing },
	_level{}
{
	PlayerId_t playerIds[MAX_PLAYERS];
	
	for (int idx = 0; idx < MAX_PLAYERS; ++idx)
	{
		playerIds[idx] = players[idx]->GetSession().GetPlayerId();
		
		_playerIdToPlayerIdxMap.emplace(playerIds[idx], idx);
		_player[idx] = players[idx];
	}

	_level.InitLevel(playerIds);

	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.GetGameObjects())
	{
		spdlog::debug("[Game::Game] object: {}", gameObject->GetClassId());

		auto networkId = networkManager.RegisterGameObject(gameObject);

		for (int idx = 0; idx < MAX_PLAYERS; ++idx)
		{
			players[idx]->GetReplicationManager().ReplicateCreate(networkId, gameObject->GetAllStateMask());
		}
	}

	SetNextReplicationTimeFromNow();
}

Game::~Game()
{
	auto& networkManager = NetworkManagerServer::GetInstance();
	for (auto gameObject : _level.GetGameObjects())
	{
		auto networkId = gameObject->GetNetworkId();
		networkManager.UnregisterGameObject(networkId);
		for (int playerIdx = 0; playerIdx < MAX_PLAYERS; ++playerIdx)
		{
			_player[playerIdx]->GetReplicationManager().ReplicateDestroy(networkId);
		}
	}
}

void Game::SetClientReady(const PlayerId_t playerId)
{
	int playerIdx = _playerIdToPlayerIdxMap[playerId];

	if (_isPlayerReadyToGoNextState[playerIdx])
	{
		spdlog::warn("[Game::SetClientReady] player is already ready. playerId: {}", playerId);
		return;
	}

	_isPlayerReadyToGoNextState[playerIdx] = true;

	if (++_numPlayersReadyCount == MAX_PLAYERS)
	{
		memset(_isPlayerReadyToGoNextState, 0, sizeof(_isPlayerReadyToGoNextState));
		_numPlayersReadyCount = 0;

		StartGame();
	}
}

void Game::StartGame()
{
	_level.SetLastFixedUpdateTimeToNow();
	_gamePlayState = GamePlayState::Playing;
}

void Game::EndGame()
{
	_gamePlayState = GamePlayState::End;
}

void Game::MovePlayer(GameObject* playerCharacter)
{
	// 이 player를 RPC를 요청한 유저가 소유권이 있을까? 하는 정보는
	// NetworkManager에서 처리했고, 실제로 MovePlayer가 동작하는 과정을 구현

	spdlog::debug("[Game::MovePlayer] called. player: {}", playerCharacter->GetClassId());
}

void Game::SendOutgoingPacket()
{
	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	for (int playerIdx = 0; playerIdx < MAX_PLAYERS; ++playerIdx)
	{
		networkManagerServer.SendTo(_player[playerIdx]);
	}
}

bool Game::HasElapsedReplicationInterval()
{
	return system_clock::now() >= _nextReplicationUpdateTime;
}

void Game::SetNextReplicationTimeFromNow()
{
	std::chrono::duration<float> offset(Constant::REPLICATION_PERIOD);
	_nextReplicationUpdateTime = system_clock::now() + std::chrono::duration_cast<system_clock::duration>(offset);
}
