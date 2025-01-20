#include "ServerPCH.h"
#include "WorldServer.h"

#include "ReceiveQueue.h"
#include "SendQueue.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Update() {
	auto& receiveQueue = ReceiveQueue::GetInstance();
	auto& sendQueue = SendQueue::GetInstance();

	while (receiveQueue.Empty() == false)
	{
		std::string received = receiveQueue.Front();
		cout << received << endl;

		sendQueue.Push(received);
	}
}