#include "ServerPCH.h"
#include "GameController.h"

#include "Game.h"
#include "GameObject.h"

GameController::GameController(Game* game, PlayerId_t player1Id, PlayerId_t player2Id) :
	_game{ game }
{
	_playerIdToPlayerIdxMap.emplace(player1Id, 0);
	_playerIdToPlayerIdxMap.emplace(player2Id, 1);
}

void GameController::SetClientReady(const PlayerId_t playerId)
{
	int playerIdx = _playerIdToPlayerIdxMap[playerId];

	if (_isPlayerReadyToGoNextState[playerIdx])
	{
		spdlog::warn("[GameController::SetClientReady] player is already ready. playerId: {}", playerId);
		return;
	}

	_isPlayerReadyToGoNextState[playerIdx] = true;
	
	if (++_numPlayersReadyCount == MAX_PLAYERS)
	{
		memset(_isPlayerReadyToGoNextState, 0, sizeof(_isPlayerReadyToGoNextState));
		_numPlayersReadyCount = 0;

		_game->StartGame();
	}
}

void GameController::MovePlayer(GameObject* playerCharacter)
{
	// 이 player를 RPC를 요청한 유저가 소유권이 있을까? 하는 정보는
	// NetworkManager에서 처리했고, 실제로 MovePlayer가 동작하는 과정을 구현

	spdlog::debug("MovePlayer called. player: {}", playerCharacter->GetClassId());
}
