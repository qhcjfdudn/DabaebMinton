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

void InitLevel()
{
	auto& engineInstance = Engine::GetInstance();

	engineInstance.CreatePlain(0.f, 1.f, 0.f, 0.f);
	engineInstance.CreateBox(PxTransform{ PxVec3{ 10, 5, 0 } }, 1, 1, 1);
}

int main()
{
	auto& engineInstance = Engine::GetInstance();
	engineInstance.initPhysics();

	auto& networkInstance = NetworkManagerServer::GetInstance();
	networkInstance.InitIOCP();

	auto& worldInstance = WorldServer::GetInstance();
	worldInstance.InitWorld();

	signal(SIGINT, signalHandler);

	InitLevel();

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

	engineInstance.cleanupPhysics();
	
	// 모든 thread의 join 필요
	cout << "Server Main done." << endl;
}
