#include "ServerPCH.h"

#include "Engine.h"
#include "NetworkManagerServer.h"
#include "WorldServer.h"

int main() {
	auto& engineInstance = Engine::GetInstance();
	engineInstance.initPhysics(false);

	auto& networkInstance = NetworkManagerServer::GetInstance();
	//networkInstance.Init();
	networkInstance.InitIOCP();

	auto& worldInstance = WorldServer::GetInstance();
	worldInstance.InitWorld();

	cout << "engineInstance start" << endl;
	while (engineInstance.isRunning) {
		engineInstance.stepPhysics(true);

		//networkInstance.ReceivePackets();
		networkInstance.ProcessIOCPEvent();
		worldInstance.Update();
		worldInstance.FixedUpdate();
		worldInstance.WriteWorldStateToStream();
		//networkInstance.SendPackets();
		networkInstance.SendPacketsIOCP();
	}

	engineInstance.cleanupPhysics(true);
	cout << "engineInstance end" << endl;
}
