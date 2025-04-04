#include "ServerPCH.h"

#include <csignal>

#include "Engine.h"
#include "NetworkManagerServer.h"
#include "WorldServer.h"
#include "Constant.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;
	Engine& engineInstance = Engine::GetInstance();
	engineInstance.TurnOff();
}

int main()
{
	signal(SIGINT, signalHandler);
	
	auto& engineInstance = Engine::GetInstance();
	engineInstance.initPhysics();

	auto& networkInstance = NetworkManagerServer::GetInstance();
	networkInstance.InitIOCP();

	auto& worldInstance = WorldServer::GetInstance();
	worldInstance.InitLevel();

	thread userInputThread([] {
		auto& engineInstance = Engine::GetInstance();
		string cmd;
		while (engineInstance.isRunning)
		{
			std::getline(std::cin, cmd);
			if (cmd == "r")
			{
				WorldServer::GetInstance().RemoveAllGameObjects();
			}
			else if (cmd == "s")
			{
				WorldServer::GetInstance().InitLevel();
			}
		}
		});

	// fps 업데이트를 위해 별도 thread 동작
	// 리팩터링 적용 필요
	thread physXThread([]() {
		auto& engineInstance = Engine::GetInstance();
		system_clock::time_point _lastFixedUpdateTime = system_clock::now();

		// thread 내에서 참조하는 외부 변수. atomic으로 변경해 잠재적 동시성 오류 해결하자.
		while (engineInstance.isRunning)
		{
			system_clock::time_point currentTime = system_clock::now();
			std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;

			if (elapsedTime.count() < Constant::PHYSX_FIXED_UPDATE_TIMESTEP)
				continue;

			engineInstance.stepPhysics();

			_lastFixedUpdateTime = currentTime;
		}
		});

	while (engineInstance.isRunning) {
		networkInstance.ProcessIOCPEvent();
		worldInstance.Update();
		worldInstance.FixedUpdate();
		worldInstance.WriteWorldStateToStream();
		networkInstance.SendPacketsIOCP();
	}

	physXThread.join();
	userInputThread.join();

	worldInstance.Clear();
	engineInstance.cleanupPhysics();
	
	cout << "Server Main done." << endl;
}
