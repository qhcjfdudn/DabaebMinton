#include "ServerPCH.h"
#include "GameController.h"

#include "GameObject.h"

void GameController::MovePlayer(GameObject* playerCharacter)
{
	// 이 player를 RPC를 요청한 유저가 소유권이 있을까? 하는 정보는
	// NetworkManager에서 처리했고, 실제로 MovePlayer가 동작하는 과정을 구현

	spdlog::debug("MovePlayer called. player: {}", playerCharacter->GetClassId());
}
