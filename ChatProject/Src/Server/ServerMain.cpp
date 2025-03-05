#include "ServerPCH.h"

#include "Engine.h"
#include "NetworkManagerServer.h"
#include "WorldServer.h"

int main() {
	auto& engine = Engine::GetInstance();
	
	auto& networkInstance = NetworkManagerServer::GetInstance();
	//networkInstance.Init();
	networkInstance.InitIOCP();

	auto& worldInstance = WorldServer::GetInstance();
	worldInstance.InitWorld();

	cout << "engine start" << endl;
	while (engine.isRunning) {

		//networkInstance.ReceivePackets();
		networkInstance.ProcessIOCPEvent();
		worldInstance.Update();
		worldInstance.FixedUpdate();
		worldInstance.WriteWorldStateToPacket();
		//networkInstance.SendPackets();
		networkInstance.SendPacketsIOCP();

		//engine.isRunning = false;
	}
	cout << "engine end" << endl;
}