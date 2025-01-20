#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Update() {
	auto& packetQueue = PacketQueue::GetInstance();

	while (packetQueue.Empty() == false)
	{
		string received = packetQueue.Front();
		cout << received << endl;
	}
}