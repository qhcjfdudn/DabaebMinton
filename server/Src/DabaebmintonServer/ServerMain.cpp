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

	thread gameStepPhysicsThread([] {
		auto& serverEngine = ServerEngine::GetInstance();
		auto& gameManager = GameManager::GetInstance();

		while (serverEngine.isRunning.load(std::memory_order_acquire))
		{
			auto& games = gameManager._games;
			gameManager._gamesMutex.lock();
			for (auto game : games)
			{	
				game->StepPhysicsIfPossible();
			}
			gameManager._gamesMutex.unlock();
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

	///////////////////////////////
	// Server 종료 signal 이후 로직
	///////////////////////////////

	outgoingPacketProcessProducer.join();

	for (thread& gameReplicationUpdateConsumer : gameReplicationUpdateConsumers)
		gameReplicationUpdateConsumer.join();

	HttpServer::GetInstance().Stop();

	// Game이 사용하는 resource 반납
	GameManager::GetInstance().RemoveAllGames();

	networkEngineRunningThread.join();
	gameStepPhysicsThread.join();

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
