#include "ServerPCH.h"
#include "ReplicationUpdateProducer.h"

#include "ServerEngine.h"
#include "GameManager.h"

#include "Game.h"

void ReplicationUpdateProducer::operator() ()
{
	auto& serverEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	while (serverEngine.isRunning)
	{
		std::lock_guard gamesLock(gameManager._gamesMutex);
		for (auto game : gameManager._games)
		{
			if (game->_replicationState.load(std::memory_order_relaxed) == GameReplicationState::None &&
				game->HasElapsedReplicationInterval())
			{
				game->_replicationState.store(GameReplicationState::Pending, std::memory_order_acquire);
				
				gameManager._pendingReplicationMutex.lock();
				gameManager._pendingReplicationQueue.push(game.get());
				gameManager._pendingReplicationMutex.unlock();
				
				gameManager._replicationCv.notify_one();
			}
		}
	}
	gameManager._replicationCv.notify_all();
}