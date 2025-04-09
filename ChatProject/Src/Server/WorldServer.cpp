#include "ServerPCH.h"
#include "WorldServer.h"

#include "Engine.h"
#include "PacketQueue.h"
#include "OutputMemoryBitStream.h"
#include "ReplicationManager.h"
#include "LinkingContext.h"

#include "Constant.h"

#include "Shuttlecock.h"

#include "GetRequiredBits.h"

WorldServer& WorldServer::GetInstance() {
	static WorldServer instance;
	return instance;
}

void WorldServer::Clear()
{
	RemoveAllGameObjects();

	_pendingSerializationQueue = queue<shared_ptr<GameObject>>{};

	_linkingContext.Clear();
}

void WorldServer::InitLevel()
{
	// engine으로부터 초기 object 생성
	auto& engineInstance = Engine::GetInstance();

	engineInstance.CreatePlain(0.f, 1.f, 0.f, 0.f);
	
	// Net
	engineInstance.CreateBox2DStatic(PxVec2{ 0, 2.5 }, 0.5, 2.5);

	// Shuttlecock
	PxVec2 location(-3, 10);
	PxVec2 velocity(2, 5);
	auto shuttlecock = make_shared<Shuttlecock>(location, velocity);

	_gameObjects.push_back(shuttlecock);

	// LinkingContext에 등록
	_linkingContext.AddGameObject(shuttlecock);
}

void WorldServer::RemoveAllGameObjects() {
	for (int idx = static_cast<int>(_gameObjects.size()) - 1; idx >= 0; --idx)
		RemoveGameObject(idx);
}

// World가 참조하는 gameObject를 지운다. O(N)이라 gameObjects가 많을수록 성능 저하 발생
void WorldServer::Remove(shared_ptr<GameObject> gameObject) {
	size_t idx = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject) - _gameObjects.begin();
	
	if (idx >= _gameObjects.size())
		return;

	RemoveGameObject(idx);
}

void WorldServer::RemoveGameObject(size_t idx)
{
	auto& go = _gameObjects[idx];

	if (hasFlag(go->replicationFlag, ReplicationFlag::DF_ALL) == false)
		_pendingSerializationQueue.push(go);

	go->SetDirtyFlag(ReplicationFlag::DF_DELETE);
	
	swap(go, _gameObjects.back());
	_gameObjects.pop_back();
}

void WorldServer::Update()
{
	// 아래 receive packet을 다루는 코드는 추후 ReplicationManager와 함께
	// NetworkManager에서 동작하도록 변경
	auto& receiveQueue = PacketQueue::GetReceiveStaticInstance();
	auto& sendQueue = PacketQueue::GetSendStaticInstance();

	while (receiveQueue.Empty() == false)
	{
		shared_ptr<Packet> received = receiveQueue.Front();

		const local_time<system_clock::duration> now = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
		cout << "[" << now << "] received: " << received->GetBuffer() << endl;

		sendQueue.PushCopy(*received);
	}
}

void WorldServer::FixedUpdate() {
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;

	if (elapsedTime.count() < Constant::FIXED_UPDATE_TIMESTEP)
		return;

	_lastFixedUpdateTime = currentTime;

	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	cout << "[" << now << "] FixedUpdate" << endl;

	// 아래 코드가 안정성을 보장하는지 검증 필요
	// ex) _gameObjects의 복사 중 _gameObjects의 요소의 추가/변경/삭제가 발생한다면?
	auto gameObjects = _gameObjects;
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->FixedUpdate() == false)
			continue;
		
		ReplicationFlag& replicationFlag = gameObject->replicationFlag;

		if (hasFlag(replicationFlag, (ReplicationFlag::DF_UPDATE | ReplicationFlag::DF_DELETE)))
			continue;

		gameObject->SetDirtyFlag(ReplicationFlag::DF_UPDATE);
		_pendingSerializationQueue.push(gameObject);
	}
}

void WorldServer::WriteWorldStateToStream()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastPacketUpdateTime;

	if (elapsedTime.count() < Constant::PACKET_PERIOD)
		return;

	_lastPacketUpdateTime = currentTime;
	
	if (_pendingSerializationQueue.empty())
		return;

	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	cout << "[" << now << "] WriteWorldStateToStream" << endl;
	cout << "pendingSize: " << _pendingSerializationQueue.size() << endl;

	OutputMemoryBitStream outStream;
	outStream.WriteBits(static_cast<int>(PacketType::PT_ReplicationData),
		GetRequiredBits(static_cast<int>(PacketType::PT_Max)));

	int replicatedObjectCount = WriteByReplication(outStream);

	if (replicatedObjectCount <= 0)
		return;

	cout << "outStream.GetBitLength(): " << outStream.GetBitLength() << endl;
	cout << "outStream.GetByteLength(): " << outStream.GetByteLength() << endl;

	Packet packet{ outStream.GetBufferPtr(), outStream.GetByteLength() };

	packet.PrintInHex();

	PacketQueue::GetSendStaticInstance()
		.PushCopy(packet);
}

int WorldServer::WriteByReplication(OutputMemoryBitStream& outStream)
{
	auto& replicationManager = ReplicationManager::GetInstance();
	int pendingSerializationCount = static_cast<int>(_pendingSerializationQueue.size());

	for (int i = 0; i < pendingSerializationCount; ++i) {
		shared_ptr<GameObject> go = _pendingSerializationQueue.front();
		_pendingSerializationQueue.pop();

		ReplicationFlag& replicationFlag = go->replicationFlag;

		if (hasFlag(replicationFlag, ReplicationFlag::DF_DELETE)) {
			cout << "DF_DELETE" << endl;
			replicationManager.ReplicateDelete(outStream, go);
			_linkingContext.RemoveGameObject(go);
		}
		else if (hasFlag(replicationFlag, ReplicationFlag::DF_UPDATE)) {
			replicationManager.ReplicateUpdate(outStream, go);
		}
		else if (hasFlag(replicationFlag, ReplicationFlag::DF_CREATE)) {
			//replicationManager.ReplicateCreate(outStream, go);
			//_linkingContext.AddGameObject(go);
		}

		replicationFlag = ReplicationFlag::DF_NONE;
	}

	return pendingSerializationCount;
}

WorldServer::WorldServer() :
	_linkingContext(LinkingContext::GetInstance())
{ 
	_lastFixedUpdateTime = _lastPacketUpdateTime = system_clock::now();
}
