#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"
#include "Shuttlecock.h"
#include "LinkingContext.h"
#include "Constant.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::InitWorld()
{
	// Shuttlecock 만들기
	Vector2 position(0, 0);
	Vector2 velocity(0, 1);
	auto shuttlecock = make_shared<Shuttlecock>(position, velocity);
	
	_gameObjects.push_back(shuttlecock);

	// LinkingContext에 등록
	auto& linkingContext = LinkingContext::GetInstance();
	linkingContext.AddGameObject(shuttlecock);

	_lastFixedUpdateTime = system_clock::now();
}

void WorldServer::FixedUpdate() {
	// 아래 receive packet을 다루는 코드는 추후 ReplicationManager와 함께
	// NetworkManager에서 동작하도록 변경
	auto& receiveQueue = PacketQueue::GetReceiveStaticInstance();
	auto& sendQueue = PacketQueue::GetSendStaticInstance();

	while (receiveQueue.Empty() == false)
	{
		std::string received = receiveQueue.Front();

		const local_time<system_clock::duration> now = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
		cout << "[" << now << "] received: " << received << endl;

		sendQueue.Push(received);
	}

	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;
	if (elapsedTime.count() >= Constant::FIXED_TIMESTEP)
	{
		_lastFixedUpdateTime = currentTime;
		
		const local_time<system_clock::duration> now = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
		cout << "[" << now << "] FixedUpdate" << endl;

		for (auto& gameObject : _gameObjects)
		{
			gameObject->FixedUpdate();
			// replication update code here (send packet)
		}

	}
}

