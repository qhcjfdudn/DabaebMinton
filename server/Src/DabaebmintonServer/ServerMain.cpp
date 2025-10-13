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

#include "OutgoingPacketProcessProducer.h"
#include "OutgoingPacketProcessConsumer.h"

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
		auto& serverEngine = ServerEngine::GetInstance();
		auto& networkManagerServer = NetworkManagerServer::GetInstance();

		while (serverEngine.isRunning.load(std::memory_order_acquire))
		{
			networkManagerServer.ProcessQueuedPackets();
			networkManagerServer.ProcessIOCPEvent();
		}
		});

	thread physicsEngineRunningThread([] {
		auto& serverEngine = ServerEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		// game이 Playing 상태일 때만 물리 연산 수행하도록 변경 필요

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
	OutgoingPacketProcessProducer outgoingPacketProcessProducerFunctor;
	thread outgoingPacketProcessProducer(outgoingPacketProcessProducerFunctor);

	OutgoingPacketProcessConsumer replicationUpdateConsumer;
	vector<thread> gameReplicationUpdateConsumers;
	for (int i = 0; i < 4; ++i)
		gameReplicationUpdateConsumers.emplace_back(replicationUpdateConsumer);

	// 서버 검증을 위한 커맨드 처리용 Thread
	DeveloperCommandFunctor developerCommandFunctor;
	thread developerInputThread(developerCommandFunctor);

	///////////////////////////////
	// Server 종료 signal 이후 로직
	///////////////////////////////

	developerInputThread.join();

	HttpServer::GetInstance().Stop();
	
	outgoingPacketProcessProducer.join();

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
