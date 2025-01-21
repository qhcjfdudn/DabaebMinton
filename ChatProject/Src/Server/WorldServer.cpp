#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Update() {
	auto& receiveQueue = PacketQueue::GetReceiveStaticInstance();
	auto& sendQueue = PacketQueue::GetSendStaticInstance();

	while (receiveQueue.Empty() == false)
	{
		std::string received = receiveQueue.Front();
		cout << "received: " << received << endl;

		sendQueue.Push(received);
	}
}