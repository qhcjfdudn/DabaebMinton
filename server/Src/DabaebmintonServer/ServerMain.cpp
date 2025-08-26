#include "ServerPCH.h"

#include <csignal>

#include "Engine.h"
#include "NetworkManagerServer.h"
#include "ReplicationManager.h"

#include "Constant.h"
#include "Level.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;
	auto& engineInstance = Engine::GetInstance();
	engineInstance.TurnOff();
}

int main()
{
	SetConsoleOutputCP(CP_UTF8);
	signal(SIGINT, signalHandler);
	
	auto& engineInstance = Engine::GetInstance();
	engineInstance.InitPhysics();

	shared_ptr<ReplicationManager> replicationManager = make_shared<ReplicationManager>();
	auto& networkInstance = NetworkManagerServer::GetInstance();
	networkInstance.SetReplicationManager(replicationManager);
	networkInstance.InitIOCP();

	vector<Level> levels(1);
	levels[0].InitLevel();

	thread networkThread([&networkInstance, &levels] {
		auto& engineInstance = Engine::GetInstance();
		while (engineInstance.isRunning)
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

	// 서버 검증을 위한 커맨드 처리용 Thread
	thread developerInputThread([&levels] {
		auto& engineInstance = Engine::GetInstance();
		string cmd;
		while (engineInstance.isRunning)
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

	thread physXThread([&levels]() {
		auto& engineInstance = Engine::GetInstance();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (engineInstance.isRunning)
		{
			for (auto& level : levels)
			{
				system_clock::time_point currentTime = system_clock::now();
				std::chrono::duration<double> elapsedTime = currentTime - engineInstance.lastPhysxFixedUpdateTime;

				if (elapsedTime.count() < Constant::PHYSX_FIXED_UPDATE_TIMESTEP)
					continue;

				engineInstance.lastPhysxFixedUpdateTime = currentTime;

				level.StepPhysics();
			}
		}
		});

	thread levelPlayThread([&levels] {
		auto& engineInstance = Engine::GetInstance();

		while (engineInstance.isRunning) {
			for (auto& level : levels) {
				level.FixedUpdate();
				//level.WriteWorldStateToStream();
			}
		}

		for (auto& level : levels)
			level.Release();
		});

	developerInputThread.join();
	levelPlayThread.join();
	networkThread.join();
	physXThread.join();

	engineInstance.CleanupPhysics();
	
	cout << "Server Main done." << endl;
}
