#include "ServerPCH.h"
#include "ReplicationUpdateConsumer.h"

#include "ServerEngine.h"
#include "NetworkManagerServer.h"
#include "GameManager.h"

#include "Game.h"

void ReplicationUpdateConsumer::operator() ()
{
	auto& serverEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	while (serverEngine.isRunning.load(std::memory_order_acquire))
	{
		std::unique_lock lk(gameManager._pendingReplicationMutex, std::defer_lock);
		lk.lock();
		gameManager._replicationCv.wait(lk, [&] {
			return serverEngine.isRunning.load(std::memory_order_acquire) == false
				|| gameManager._pendingReplicationQueue.empty() == false; });

		if (serverEngine.isRunning.load(std::memory_order_acquire) == false)
			break;

		Game* game = gameManager._pendingReplicationQueue.front();
		gameManager._pendingReplicationQueue.pop();
		lk.unlock();

		auto& networkManager = NetworkManagerServer::GetInstance();
		networkManager.SendTo(game->p_player1);
		networkManager.SendTo(game->p_player2);

		game->_replicationState.store(GameReplicationState::None, std::memory_order_release);
	}
}