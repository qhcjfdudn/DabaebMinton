#include "ServerPCH.h"
#include "WorldServer.h"

#include "ReceiveQueue.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Update() {
	auto& packetQueue = ReceiveQueue::GetInstance();

	while (packetQueue.Empty() == false)
	{
		string received = packetQueue.Front();
		cout << received << endl;
	}
}