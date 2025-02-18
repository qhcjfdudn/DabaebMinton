#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"
#include "Shuttlecock.h"
#include "LinkingContext.h"

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

		const local_time<system_clock::duration> now = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
		cout << "[" << now << "] received: " << received << endl;

		sendQueue.Push(received);
	}
}

void WorldServer::InitWorld()
{
	// Shuttlecock 만들기
	Vector2 position(0, 0);
	auto shuttlecock = make_shared<Shuttlecock>(position);
	shuttlecock->setRadius(10);
	
	_gameObjects.push_back(shuttlecock);

	// LinkingContext에 등록
	auto& linkingContext = LinkingContext::GetInstance();

	unsigned int networkId = linkingContext.GetNetworkId(shuttlecock);
	linkingContext.AddGameObject(shuttlecock, networkId);

}
