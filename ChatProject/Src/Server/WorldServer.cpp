#include "ServerPCH.h"
#include "WorldServer.h"

#include "PacketQueue.h"
#include "OutputMemoryBitStream.h"
#include "ReplicationManager.h"
#include "LinkingContext.h"

#include "Constant.h"

#include "Shuttlecock.h"

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
	std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;

	if (elapsedTime.count() < Constant::FIXED_UPDATE_TIMESTEP)
		return;

	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	cout << "[" << now << "] FixedUpdate" << endl;

	for (auto& gameObject : _gameObjects)
	{
		NetworkId_t networkId = gameObject->GetNetworkId();
		if (gameObject->FixedUpdate() && 
			_updatedObjectNetworkIds.find(networkId) == _updatedObjectNetworkIds.end())
		{
			_updatedObjectNetworkIds.insert(networkId);
			_pendingSerializationQueue.push(networkId);
		}
	}

	_lastFixedUpdateTime = currentTime;
}

void WorldServer::WriteWorldStateToStream()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastPacketUpdateTime;

	if (elapsedTime.count() < Constant::PACKET_PERIOD)
		return;

	_lastPacketUpdateTime = currentTime;

	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	cout << "[" << now << "] WriteWorldStateToStream" << endl;

	auto& linkingContext = LinkingContext::GetInstance();
	auto& replicationManager = ReplicationManager::GetInstance();
	OutputMemoryBitStream inStream;

	// delta가 있는 객체만 Update 하고 싶다.
	// => Update가 있는 객체의 GUID를 기록한 Queue로 구현
	// 큐의 원소를 모두 pop 하면서 stream에 기록
	// PacketQueue에 넣을 때 stream 값이 복사되기 때문에, 여기서 stream을 생성하고 소멸시켜도
	// 문제 없다.
	int pending_serialization_count = static_cast<int>(_pendingSerializationQueue.size());
	while (pending_serialization_count--)
	{
		NetworkId_t networkId = _pendingSerializationQueue.front();
		_pendingSerializationQueue.pop();

		auto gameObject = linkingContext.GetGameObject(networkId);
		replicationManager.ReplicateUpdate(inStream, gameObject);

		_updatedObjectNetworkIds.erase(networkId);
	}

	if (inStream.GetBitLength() <= 0)
		return;

	auto& sendQueue = PacketQueue::GetSendStaticInstance();
	sendQueue.Push(inStream.GetBufferPtr());
}

