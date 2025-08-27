#include "ServerPCH.h"

#include <csignal>

#include "GameEngine.h"
#include "PhysicsEngine.h"
#include "NetworkManagerServer.h"
#include "ReplicationManager.h"

#include "Constant.h"
#include "Level.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;
	
	GameEngine::GetInstance().TurnOff();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	signal(SIGINT, signalHandler);
	
	auto& gameEngine = GameEngine::GetInstance();

	shared_ptr<ReplicationManager> replicationManager = make_shared<ReplicationManager>();
	auto& networkInstance = NetworkManagerServer::GetInstance();
	networkInstance.SetReplicationManager(replicationManager);
	networkInstance.InitIOCP();

	thread networkThread([&networkInstance] {
		auto& gameEngine = GameEngine::GetInstance();
		
		while (gameEngine.isRunning)
		{
			networkInstance.ProcessIOCPEvent();

			if (networkInstance.HasElapsedPacketInterval())
			{
				networkInstance.ReplicateAllGameObjects();

				// 채널 별로 interval이 필요한 것인가.
				networkInstance.ResetPacketTimer();
			}
		}

		networkInstance.RemoveAllGameObjectsForReplication();

		});

	thread physicsThread([]() {
		auto& gameEngine = GameEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		physicsEngine.InitPhysics();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (gameEngine.isRunning)
		{
			system_clock::time_point currentTime = system_clock::now();
			std::chrono::duration<double> elapsedTime = currentTime - physicsEngine.lastPhysxFixedUpdateTime;

			if (elapsedTime.count() < Constant::PHYSX_FIXED_UPDATE_TIMESTEP)
				continue;

			physicsEngine.StepPhysicsEveryScene();

			physicsEngine.lastPhysxFixedUpdateTime = currentTime;
		}

		physicsEngine.CleanupPhysics();
		});

	vector<Level> levels(1);
	levels[0].InitLevel();

	thread levelPlayThread([&levels] {
		auto& gameEngine = GameEngine::GetInstance();

		while (gameEngine.isRunning) {
			for (auto& level : levels) {
				level.FixedUpdate();
				//level.WriteWorldStateToStream();
			}
		}

		for (auto& level : levels)
			level.Release();
		});

	// 서버 검증을 위한 커맨드 처리용 Thread
	thread developerInputThread([&levels] {
		auto& gameEngine = GameEngine::GetInstance();
		string cmd;
		while (gameEngine.isRunning)
		{
			std::getline(std::cin, cmd);
			if (cmd == "r")
			{
				for (auto& level : levels)
					level.RemoveAllGameObjects();
			}
			else if (cmd == "s")
			{
				for (auto& level : levels)
					level.InitLevel();
			}
		}
		});

	developerInputThread.join();
	levelPlayThread.join();
	physicsThread.join();
	networkThread.join();
	
	cout << "Server Main done." << endl;
}
