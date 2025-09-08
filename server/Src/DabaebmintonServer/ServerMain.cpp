#include "ServerPCH.h"

#include <csignal>

#include "ServerEngine.h"
#include "NetworkManagerServer.h"
#include "PhysicsEngine.h"
#include "HttpServer.h"
#include "Observer.h"

#include "GameManager.h"
#include "Game.h"
#include "Level.h"

#include "ReplicationUpdateProducer.h"
#include "ReplicationUpdateConsumer.h"

#include "DeveloperCommandFunctor.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;

	Observer::notify(ObserverEvent::EngineOff);
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

	thread httpServerInitThread([] {
		HttpServer::GetInstance().Init();
		});

	networkEngineInitThread.join();
	physicsEngineInitThread.join();
	httpServerInitThread.join();

	// Engine working
	thread networkEngineRunningThread([] {
		auto& networkManagerServer = NetworkManagerServer::GetInstance();
		auto& serverEngine = ServerEngine::GetInstance();

		while (serverEngine.isRunning.load(std::memory_order_acquire))
		{
			networkManagerServer.ProcessIOCPEvent();

			if (networkManagerServer.HasElapsedPacketInterval())
			{
				//networkManagerServer.ReplicateAllGameObjects();

				// 채널 별로 interval이 필요한 것인가.
				networkManagerServer.SetLastPacketSendTimeToNow();
			}
		}
		});

	thread physicsEngineRunningThread([] {
		auto& serverEngine = ServerEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		auto& scenes = physicsEngine.scenes;
		auto& lastTimes = physicsEngine._lastPhysXFixedUpdateTimeArray;

		while (serverEngine.isRunning.load(std::memory_order_acquire))
		{
			std::lock_guard lk(physicsEngine.scenesMutex);
			int len = static_cast<int>(scenes.size());
			for (int i = 0; i < len; ++i)
			{
				physicsEngine.StepPhysicsIfHasElapsedPhysicsFixedUpdateInterval(scenes[i], lastTimes[i]);
			}
		}
		});

	thread httpServerRunningThread([] {
		HttpServer::GetInstance().ListenBlock();
		});

	// Level replication update producer-consumer working
	ReplicationUpdateProducer replicationUpdateProducerFunctor;
	thread replicationUpdateProducer(replicationUpdateProducerFunctor);

	ReplicationUpdateConsumer replicationUpdateConsumer;
	vector<thread> gameReplicationUpdateConsumers;
	for (int i = 0; i < 4; ++i)
		gameReplicationUpdateConsumers.emplace_back(replicationUpdateConsumer);

	// Level running - 이 thread가 없어도 되는 것 같다. 왜냐하면? level을 유지하지 않기 때문.
	thread levelPlayThread([] {
		vector<Level> levels(1);
		levels[0].InitLevel();

		// 서버 검증을 위한 커맨드 처리용 Thread
		DeveloperCommandFunctor developerCommandFunctor(levels);
		thread developerInputThread(developerCommandFunctor);

		auto& gameEngine = ServerEngine::GetInstance();

		while (gameEngine.isRunning.load(std::memory_order_acquire))
		{
			for (auto& level : levels)
			{
				if (level.HasElapsedFixedUpdateInterval())
				{
					level.FixedUpdate();
					level.SetLastFixedUpdateTimeToNow();
				}
			}
		}

		developerInputThread.join();

		for (auto& level : levels)
			level.Release();
		});

	replicationUpdateProducer.join();

	for (thread& gameReplicationUpdateConsumer : gameReplicationUpdateConsumers)
		gameReplicationUpdateConsumer.join();

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

	httpServerRunningThread.join();

	cout << "Server Main done." << endl;
}
