#include "ServerPCH.h"

#include <csignal>

#include "GameEngine.h"
#include "ReplicationManager.h"
#include "NetworkManagerServer.h"
#include "PhysicsEngine.h"

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

	thread networkThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
		shared_ptr<ReplicationManager> replicationManager = make_shared<ReplicationManager>();
		networkInstance.SetReplicationManager(replicationManager);
		networkInstance.InitIOCP();

		auto& gameEngine = GameEngine::GetInstance();
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

		networkInstance.RemoveAllGameObjectsForReplication();

		});

	thread physicsThread([]() {
		auto& gameEngine = GameEngine::GetInstance();
		auto& physicsEngine = PhysicsEngine::GetInstance();

		physicsEngine.InitPhysics();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (gameEngine.isRunning)
		{
			if (physicsEngine.HasElapsedPhysicsUpdateInterval()) {
				physicsEngine.StepPhysicsEveryScene();
				physicsEngine.SetLastUpdateTimeToNow();
			}
		}

		physicsEngine.CleanupPhysics();
		});

	auto& physEngine = PhysicsEngine::GetInstance();

	while (physEngine.GetEngineRunningState() != PhysicsEngineRunningState::Running)
	{
		Sleep(10);
	}

	vector<Level> levels(1);
	levels[0].InitLevel();

	thread levelPlayThread([&levels] {
		

		auto& gameEngine = GameEngine::GetInstance();

		while (gameEngine.isRunning) {
			for (auto& level : levels) {
				level.FixedUpdate();
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
