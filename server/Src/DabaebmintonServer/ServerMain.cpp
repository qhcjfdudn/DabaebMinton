#include "ServerPCH.h"

#include <csignal>

#include "ServerEngine.h"
#include "NetworkManagerServer.h"
#include "PhysicsEngine.h"

#include "GameManager.h"
#include "Game.h"
#include "Level.h"

#include "DeveloperCommandFunctor.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;

	ServerEngine::GetInstance().TurnOff();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	signal(SIGINT, signalHandler);

	// Engine Init
	thread networkEngineInitThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
		networkInstance.InitIOCP();
		});

	thread physicsEngineInitThread([] {
		PhysicsEngine::GetInstance().InitPhysics();
		});

	networkEngineInitThread.join();
	physicsEngineInitThread.join();

	// Engine and Game Working
	thread networkEngineRunningThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
		auto& gameEngine = ServerEngine::GetInstance();
		
		while (gameEngine.isRunning)
		{
			networkInstance.ProcessIOCPEvent();

			if (networkInstance.HasElapsedPacketInterval())
			{
				networkInstance.ReplicateAllGameObjects();

				// 채널 별로 interval이 필요한 것인가.
				networkInstance.SetLastPacketSendTimeToNow();
			}
		}
		});

	thread physicsEngineRunningThread([] {
		auto& gameEngine = ServerEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (gameEngine.isRunning)
		{
			if (physicsEngine.HasElapsedPhysicsUpdateInterval()) {
				physicsEngine.StepPhysicsEveryScene();
				physicsEngine.SetLastUpdateTimeToNow();
			}
		}
		});

	thread AllGamesReplicationIntervalProducerThread([]
		{
			auto& serverEngine = ServerEngine::GetInstance();
			auto& gameManager = GameManager::GetInstance();

			while (serverEngine.isRunning)
			{
				for (auto game : gameManager._games)
				{
					if (game->_replicationState.load() == GameReplicationState::None
						&& game->HasElapsedReplicationInterval())
					{
						game->_replicationState.store(GameReplicationState::Pending, std::memory_order_acquire);
						gameManager._pendingReplicationMutex.lock();
						gameManager._pendingReplicationQueue.push(game.get());
						gameManager._pendingReplicationMutex.unlock();
						gameManager._replicationCv.notify_one();
					}
				}
			}
		});

	vector<thread> gameStepProcessingThreads;
	for (int i = 0; i < 4; ++i) {
		gameStepProcessingThreads.emplace_back([]
			{
				auto& serverEngine = ServerEngine::GetInstance();
				auto& gameManager = GameManager::GetInstance();

				while (serverEngine.isRunning)
				{
					Game* game = nullptr;

					for (; game == nullptr;)
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

			});
	}

	AllGamesReplicationIntervalProducerThread.join();

	thread levelPlayThread([] {
		vector<Level> levels(1);
		levels[0].InitLevel();

		// 서버 검증을 위한 커맨드 처리용 Thread
		DeveloperCommandFunctor developerCommandFunctor(levels);
		thread developerInputThread(developerCommandFunctor);

		auto& gameEngine = ServerEngine::GetInstance();

		while (gameEngine.isRunning) {
			for (auto& level : levels) {
				if (level.HasElapsedFixedUpdateInterval()) {
					level.FixedUpdate();
					level.SetLastFixedUpdateTimeToNow();
				}
			}
		}

		developerInputThread.join();

		for (auto& level : levels)
			level.Release();
		});

	for (thread& t : gameStepProcessingThreads)
		t.join();

	networkEngineRunningThread.join();
	physicsEngineRunningThread.join();
	levelPlayThread.join();

	// Engine Turn Off with Garbage Collection
	thread physicsEngineCleaupThread([] {
		PhysicsEngine::GetInstance().CleanupPhysics();
		});
	physicsEngineCleaupThread.join();

	thread networkEngineTurnOffThread([] {
		NetworkManagerServer::GetInstance().RemoveAllGameObjectsForReplication();
		});
	networkEngineTurnOffThread.join();

	cout << "Server Main done." << endl;
}
