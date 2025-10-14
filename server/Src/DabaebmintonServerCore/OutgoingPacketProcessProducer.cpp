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
			if (game->IsPendingReplicationUpdate.load(std::memory_order_acquire) == false &&
				game->HasElapsedReplicationInterval())
			{
				game->IsPendingReplicationUpdate.store(true, std::memory_order_release);
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