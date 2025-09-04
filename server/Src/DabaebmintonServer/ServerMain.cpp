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
		auto& gameEngine = ServerEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (gameEngine.isRunning.load(std::memory_order_acquire))
		{
			if (physicsEngine.HasElapsedPhysicsUpdateInterval())
			{
				physicsEngine.StepPhysicsEveryScene();
				physicsEngine.SetLastUpdateTimeToNow();
			}
		}
		});

	thread httpServerRunningThread([] {
		HttpServer::GetInstance().ListenBlock();
		});

	// Level replication update producer-consumer working
	ReplicationUpdateProducer replicationUpdateProducer;
	thread replicationUpdateProduceWorker(replicationUpdateProducer);

	ReplicationUpdateConsumer replicationUpdateConsumer;
	vector<thread> gameReplicationUpdateConsumers;
	for (int i = 0; i < 4; ++i)
		gameReplicationUpdateConsumers.emplace_back(replicationUpdateConsumer);

	// Level running
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

	replicationUpdateProduceWorker.join();

	for (thread& t : gameReplicationUpdateConsumers)
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

	httpServerRunningThread.join();

	cout << "Server Main done." << endl;
}
