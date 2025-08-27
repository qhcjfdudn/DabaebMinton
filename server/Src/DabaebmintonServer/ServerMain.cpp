#include "ServerPCH.h"

#include <csignal>

#include "GameEngine.h"
#include "ReplicationManager.h"
#include "NetworkManagerServer.h"
#include "PhysicsEngine.h"

#include "Constant.h"
#include "Level.h"

#include "DeveloperCommandFunctor.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;

	GameEngine::GetInstance().TurnOff();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	signal(SIGINT, signalHandler);

	thread networkEngineInitThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
		networkInstance.SetReplicationManager(make_shared<ReplicationManager>());
		networkInstance.InitIOCP();
		});

	thread physicsEngineInitThread([] {
		PhysicsEngine::GetInstance().InitPhysics();
		});

	networkEngineInitThread.join();
	physicsEngineInitThread.join();

	thread networkEngineRunningThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
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
		});

	thread physicsEngineRunningThread([] {
		auto& gameEngine = GameEngine::GetInstance();
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

	thread levelPlayThread([] {
		vector<Level> levels(1);
		levels[0].InitLevel();

		// 서버 검증을 위한 커맨드 처리용 Thread
		DeveloperCommandFunctor developerCommandFunctor(levels);
		thread developerInputThread(developerCommandFunctor);

		auto& gameEngine = GameEngine::GetInstance();

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

	networkEngineRunningThread.join();
	physicsEngineRunningThread.join();
	levelPlayThread.join();

	// 기반 코드 종료 Routine 수행
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
