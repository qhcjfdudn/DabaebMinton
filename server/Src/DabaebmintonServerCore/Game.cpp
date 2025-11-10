#include "ServerPCH.h"
#include "Game.h"

#include "Constant.h"
#include "PhysicsEngine.h"
#include "NetworkManagerServer.h"
#include "GameObject.h"
#include "ClientProxy.h"

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

		gameObject->SetDirtyHandler(
			[this](NetworkId_t id, uint8_t dirtyState)
			{
				spdlog::debug("[Game::Game] MarkDirtyState called. id: {}, dirtyState: {}", id, static_cast<int>(dirtyState));

				for (int playerIdx = 0; playerIdx < MAX_PLAYERS; ++playerIdx)
				{
					_player[playerIdx]->GetReplicationManager().SetStateDirty(id, dirtyState);
				}
			});
	}

	SetNextReplicationTimeFromNow();

	SetNextStepPhysicsTime(steady_clock::now() + 3s);
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

void Game::StartGame()
{
	_level.SetLastFixedUpdateTimeToNow();

	SetNextStepPhysicsTime(steady_clock::now());
	
	_gamePlayState = GamePlayState::Playing;
}

void Game::EndGame()
{
	_gamePlayState = GamePlayState::End;
}

bool Game::HasElapsedReplicationInterval() const
{
	return system_clock::now() >= _nextReplicationUpdateTime;
}

void Game::SetNextReplicationTimeFromNow()
{
	std::chrono::duration<float> offset(Constant::REPLICATION_PERIOD);
	_nextReplicationUpdateTime = system_clock::now() + std::chrono::duration_cast<system_clock::duration>(offset);
}

void Game::SetNextStepPhysicsTime(const steady_clock::time_point& time)
{
	_lastRealStepPhysicsTime = time;

	duration<float> offset{ Constant::PHYSX_FIXED_UPDATE_TIMESTEP };
	_nextStepPhysicsPeriod = time + duration_cast<steady_clock::duration>(offset);
}

bool Game::StepPhysicsIfPossible()
{
	const auto now = steady_clock::now();
	if (HasElapsedStepPhysicsInterval(now) == false)
	{
		return false;
	}

	StepPhysics(now);

	return true;
}

bool Game::HasElapsedStepPhysicsInterval(const steady_clock::time_point& time) const
{
	return time >= _nextStepPhysicsPeriod;
}

void Game::StepPhysics(const steady_clock::time_point& curTime)
{
	_level.FixedUpdate();

	PxReal elapsed = duration_cast<duration<float>>(curTime - _lastRealStepPhysicsTime).count();
	PhysicsEngine::GetInstance().StepPhysics(_level.GetScene(), elapsed);

	_lastRealStepPhysicsTime = curTime;
	fetchNextStepPhysicsTime();
}

void Game::fetchNextStepPhysicsTime()
{
	duration<float> offset{ Constant::PHYSX_FIXED_UPDATE_TIMESTEP };
	_nextStepPhysicsPeriod += duration_cast<steady_clock::duration>(offset);
}

void Game::SendOutgoingPacket()
{
	auto& networkManagerServer = NetworkManagerServer::GetInstance();

	for (int playerIdx = 0; playerIdx < MAX_PLAYERS; ++playerIdx)
	{
		networkManagerServer.SendTo(_player[playerIdx]);
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
	
	ClientProxy* playerProxy = _player[playerIdx];
	playerProxy->GetReplicationManager().ShouldReplicate = true;

	if (++_numPlayersReadyCount == MAX_PLAYERS)
	{
		memset(_isPlayerReadyToGoNextState, 0, sizeof(_isPlayerReadyToGoNextState));
		_numPlayersReadyCount = 0;

		StartGame();
	}
}

void Game::MovePlayer(Player* character, PxVec2 direction)
{
	// movable 한지 검증할 수 있다면 좋겠지만 생략

	spdlog::debug("[Game::MovePlayer] called. player: {}", character->GetNetworkId());

	character->_moveValue = direction.x;
}
