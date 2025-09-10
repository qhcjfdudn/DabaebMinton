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

#include <spdlog/spdlog.h>

void signalHandler(int signum)
{
	spdlog::debug("[signalHandler] Interrupt signal ({}) received.", signum);

	Observer::notify(ObserverEvent::EngineOff);
}

void InitLoggingLevel()
{
#ifdef NDEBUG
#else
	spdlog::set_level(spdlog::level::debug);
#endif
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	signal(SIGINT, signalHandler);
	InitLoggingLevel();

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
	// 당장은 Worker를 만들어 Worker에서 별도로 동작하도록 구현 조치 필요.
	//thread levelPlayThread([] {
	//	auto& gameEngine = ServerEngine::GetInstance();

	//	while (gameEngine.isRunning.load(std::memory_order_acquire))
	//	{
	//		for (auto& level : levels)
	//		{
	//			if (level.HasElapsedFixedUpdateInterval())
	//			{
	//				level.FixedUpdate();
	//				level.SetLastFixedUpdateTimeToNow();
	//			}
	//		}
	//	}

	//	for (auto& level : levels)
	//		level.Release();
	//	});

	// 서버 검증을 위한 커맨드 처리용 Thread
	DeveloperCommandFunctor developerCommandFunctor;
	thread developerInputThread(developerCommandFunctor);

	///////////////////////////////
	// Server 종료 signal 이후 로직
	///////////////////////////////

	developerInputThread.join();

	//levelPlayThread.join();
	
	replicationUpdateProducer.join();

	for (thread& gameReplicationUpdateConsumer : gameReplicationUpdateConsumers)
		gameReplicationUpdateConsumer.join();

	// Game이 사용하는 resource 반납
	GameManager::GetInstance().RemoveAllGames();

	networkEngineRunningThread.join();
	physicsEngineRunningThread.join();

	// Engine Turn Off with Garbage Collection
	thread physicsEngineCleaupThread([] {
		PhysicsEngine::GetInstance().CleanupPhysics();
		});
	physicsEngineCleaupThread.join();

	thread networkEngineTurnOffThread([] {
		// NetworkEngine이 origin을 갖고 있는 멤버의 release 필요
		});
	networkEngineTurnOffThread.join();

	httpServerRunningThread.join();

	spdlog::info("[ServerMain::main] Server Main done.");
}
