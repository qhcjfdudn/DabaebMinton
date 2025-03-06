#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"
#include "Shuttlecock.h"
#include "ReplicationManager.h"
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

	_lastFixedUpdateTime = _lastPacketUpdateTime = system_clock::now();
}

void WorldServer::Update()
{
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
}

void WorldServer::FixedUpdate() {
	system_clock::time_point currentTime = system_clock::now();
	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;

	if (elapsedTime.count() < Constant::FIXED_UPDATE_TIMESTEP)
		return;

	cout << "[" << now << "] FixedUpdate" << endl;

	for (auto& gameObject : _gameObjects)
	{
		gameObject->FixedUpdate();
		// replication update code here (send packet)
	}

	_lastFixedUpdateTime = currentTime;
}

void WorldServer::WriteWorldStateToStream()
{
	system_clock::time_point currentTime = system_clock::now();
	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	std::chrono::duration<double> elapsedTime = currentTime - _lastPacketUpdateTime;

	if (elapsedTime.count() < Constant::PACKET_PERIOD)
		return;

	cout << "[" << now << "] WriteWorldStateToStream" << endl;

	// replication update code here (send packet)
	auto& replicationManager = ReplicationManager::GetInstance();

	// delta가 있는 객체만 Update 하고 싶다.
	// => Update가 있는 객체의 GUID를 기록한 Queue로 구현
	// 
	//replicationManager.ReplicateUpdate()

	_lastPacketUpdateTime = currentTime;
}

