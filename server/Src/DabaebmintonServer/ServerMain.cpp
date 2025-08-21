#include "ServerPCH.h"

#include <csignal>

#include "Engine.h"
#include "NetworkManagerServer.h"

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

	thread networkThread([] {
		auto& networkInstance = NetworkManagerServer::GetInstance();
		networkInstance.InitIOCP();

		shared_ptr<ReplicationManager> replicationManager = make_shared<ReplicationManager>();
		networkInstance.SetReplicationManager(replicationManager);

		auto& engineInstance = Engine::GetInstance();
		while (engineInstance.isRunning)
		{
			networkInstance.ProcessIOCPEvent();
			networkInstance.SendPacketsIOCP();
		}
		});

	vector<Level> levels(1);
	levels[0].InitLevel();

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
				std::chrono::duration<double> elapsedTime = currentTime - level.lastPhysxFixedUpdateTime;

				if (elapsedTime.count() < Constant::PHYSX_FIXED_UPDATE_TIMESTEP)
					continue;

				level.lastPhysxFixedUpdateTime = currentTime;

				level.StepPhysics();
			}
		}
		});

	thread levelPlayThread([&levels] {
		auto& engineInstance = Engine::GetInstance();

		while (engineInstance.isRunning) {
			for (auto& level : levels) {
				level.FixedUpdate();
				level.WriteWorldStateToStream();
			}
		}

		for (auto& level : levels)
			level.Release();
		});

	levelPlayThread.join();
	physXThread.join();
	developerInputThread.join();
	networkThread.join();

	engineInstance.CleanupPhysics();
	
	cout << "Server Main done." << endl;
}
