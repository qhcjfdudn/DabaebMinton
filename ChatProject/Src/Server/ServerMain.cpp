#include "ServerPCH.h"

#include <csignal>

#include "Engine.h"
#include "NetworkManagerServer.h"
#include "WorldServer.h"

#include "Constant.h"

#include "EngineNew.h"
#include "Level.h"

void signalHandler(int signum)
{
	cout << "\nInterrupt signal (" << signum << ") received." << endl;
	//Engine& engineInstance = Engine::GetInstance();
	auto& engineInstance = EngineNew::GetInstance();
	engineInstance.TurnOff();
}

int main()
{
	signal(SIGINT, signalHandler);
	
	//auto& engineInstance = Engine::GetInstance();
	auto& engineInstance = EngineNew::GetInstance();
	engineInstance.initPhysics();

	auto& networkInstance = NetworkManagerServer::GetInstance();
	networkInstance.InitIOCP();

	// World를 Level Class를 새로 만들어 replace. ServerMain에서 Level을 vector로 갖는 형태로 변경.

	//auto& worldInstance = WorldServer::GetInstance();
	//worldInstance.InitLevel();

	vector<Level> levels(1);
	levels[0].InitLevel();

	thread userInputThread([&levels] {
		//auto& engineInstance = Engine::GetInstance();
		auto& engineInstance = EngineNew::GetInstance();
		string cmd;
		while (engineInstance.isRunning)
		{
			std::getline(std::cin, cmd);
			if (cmd == "r")
			{
				//WorldServer::GetInstance().RemoveAllGameObjects();
				for (auto& level : levels)
					level.RemoveAllGameObjects();
			}
			else if (cmd == "s")
			{
				//WorldServer::GetInstance().InitLevel();
				for (auto& level : levels)
					level.InitLevel();
			}
		}
		});

	// fps 업데이트를 위해 별도 thread 동작
	// 리팩터링 적용 필요
	thread physXThread([&levels]() {
		//auto& engineInstance = Engine::GetInstance();
		auto& engineInstance = EngineNew::GetInstance();

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

				//engineInstance.stepPhysics();
				level.StepPhysics();
			}
		}
		});

	// Networking 동작과 world 동작을 별개 thread로 분리 필요
	while (engineInstance.isRunning) {
		networkInstance.ProcessIOCPEvent();
		
		//worldInstance.Update();
		//worldInstance.FixedUpdate();
		//worldInstance.WriteWorldStateToStream();
		
		for (auto& level : levels) {
			level.Update();
			level.FixedUpdate();
			level.WriteWorldStateToStream();
		}

		networkInstance.SendPacketsIOCP();
	}

	physXThread.join();
	userInputThread.join();

	//worldInstance.Clear();
	for (auto& level : levels)
		level.Release();
	
	engineInstance.cleanupPhysics();
	
	cout << "Server Main done." << endl;
}
