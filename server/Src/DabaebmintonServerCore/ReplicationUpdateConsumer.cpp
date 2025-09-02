#include "ServerPCH.h"
#include "ReplicationUpdateConsumer.h"

#include "ServerEngine.h"
#include "GameManager.h"

#include "Game.h"

void ReplicationUpdateConsumer::operator() ()
{
	auto& serverEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	while (serverEngine.isRunning.load(std::memory_order_acquire))
	{
		Game* game = nullptr;

		while (game == nullptr)
		{
			std::unique_lock lk(gameManager._pendingReplicationMutex);
			if (gameManager._pendingReplicationQueue.empty())
			{
				gameManager._replicationCv.wait(lk);
			}

			if (gameManager._pendingReplicationQueue.empty())
				continue;

			game = gameManager._pendingReplicationQueue.front();
			gameManager._pendingReplicationQueue.pop();
		}

		game->ReplicateLevel();
		game->SetLastReplicationTimeToNow();
		game->_replicationState.store(GameReplicationState::None, std::memory_order_release);
	}
}