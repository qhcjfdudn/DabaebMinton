#include "ServerPCH.h"
#include "OutgoingPacketProcessConsumer.h"

#include "ServerEngine.h"
#include "GameManager.h"
#include "Game.h"

void OutgoingPacketProcessConsumer::operator() ()
{
	auto& serverEngine = ServerEngine::GetInstance();
	auto& gameManager = GameManager::GetInstance();

	while (serverEngine.isRunning.load(std::memory_order_acquire))
	{
		std::unique_lock lk(gameManager._pendingOutgoingPacketProcessMutex, std::defer_lock);
		lk.lock();
		gameManager._pendingOutgoingPacketProcessCv.wait(lk, [&] {
			return serverEngine.isRunning.load(std::memory_order_acquire) == false
				|| gameManager._pendingOutgoingPacketProcessQueue.empty() == false; });

		if (serverEngine.isRunning.load(std::memory_order_acquire) == false)
			break;

		Game* game = gameManager._pendingOutgoingPacketProcessQueue.front();
		gameManager._pendingOutgoingPacketProcessQueue.pop();
		lk.unlock();

		game->SendOutgoingPacket();
		game->IsPendingReplicationUpdate.store(false, std::memory_order_release);
	}
}
