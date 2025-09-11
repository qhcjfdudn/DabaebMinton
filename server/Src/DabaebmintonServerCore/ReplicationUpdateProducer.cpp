#include "ServerPCH.h"
#include "ReplicationUpdateProducer.h"

#include "ServerEngine.h"
#include "GameManager.h"

#include "Game.h"

void ReplicationUpdateProducer::operator() ()
{
	auto& serverEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	while (serverEngine.isRunning.load(std::memory_order_acquire))
	{
		std::lock_guard gamesLock(gameManager._gamesMutex);
		for (auto game : gameManager._games)
		{
			if (game->_replicationState.load(std::memory_order_acquire) == GameReplicationState::None &&
				game->HasElapsedReplicationInterval())
			{
				game->_replicationState.store(GameReplicationState::Pending, std::memory_order_release);
				game->SetNextReplicationTimeFromNow();
				
				gameManager._pendingReplicationMutex.lock();
				gameManager._pendingReplicationQueue.push(game.get());
				gameManager._pendingReplicationMutex.unlock();
				
				gameManager._replicationCv.notify_one();
			}
		}

		// 추후 _games를 ReplicationUpdate 위해서 순회하는 게 아니라,
		// GameManager에서 Playing 상태 game만 array 구성한 뒤
		// 이 array를 순회하도록 변경 필요
	}
	gameManager._replicationCv.notify_all();
}