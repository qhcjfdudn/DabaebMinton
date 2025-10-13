#include "ServerPCH.h"
#include "OutgoingPacketProcessProducer.h"

#include "ServerEngine.h"
#include "GameManager.h"

#include "Game.h"

void OutgoingPacketProcessProducer::operator() ()
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
				
				gameManager._pendingOutgoingPacketProcessMutex.lock();
				gameManager._pendingOutgoingPacketProcessQueue.push(game.get());
				gameManager._pendingOutgoingPacketProcessMutex.unlock();
				
				gameManager._pendingOutgoingPacketProcessCv.notify_one();
			}
		}
	}
	gameManager._pendingOutgoingPacketProcessCv.notify_all();
}