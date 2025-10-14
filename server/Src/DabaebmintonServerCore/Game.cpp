#include "ServerPCH.h"
#include "Game.h"

#include "NetworkManagerServer.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
#include "Constant.h"
#include "ClientProxy.h"
#include "Packet.h"
#include "GameObject.h"

Game::Game(ClientProxy* player1, ClientProxy* player2) :
	_gamePlayState{ GamePlayState::Initializing },
	_level{}
{
	_playerIdToPlayerIdxMap.emplace(player1->GetSession().GetPlayerId(), 0);
	_player[0] = player1;
	_playerIdToPlayerIdxMap.emplace(player2->GetSession().GetPlayerId(), 1);
	_player[1] = player2;

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
	_gamePlayState = GamePlayState::Playing;
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
