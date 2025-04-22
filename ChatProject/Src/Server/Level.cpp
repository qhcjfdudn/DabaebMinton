#include "ServerPCH.h"
#include "Level.h"

#include "EngineNew.h"

#include "Constant.h"

#include "PacketQueue.h"
#include "OutputMemoryBitStream.h"
#include "GetRequiredBits.h"
#include "ReplicationManagerNew.h"

#include "ShuttlecockNew.h"

Level::Level()
{
	auto& engineInstance = EngineNew::GetInstance();
	
	PxSceneDesc sceneDesc(engineInstance.GetTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = engineInstance.GetCpuDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	
	pxScene = engineInstance.CreateScene(sceneDesc);

	if (pxScene == nullptr)
	{
		cout << "PxScene 생성 실패" << endl;
		return;
	}

	PxPvdSceneClient* pvdClient = pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	lastFixedUpdateTime = lastPacketUpdateTime = lastPhysxFixedUpdateTime = system_clock::now();
}

Level::~Level()
{
	PX_RELEASE(pxScene);
}

void Level::InitLevel()
{
	auto& engineInstance = EngineNew::GetInstance();
	auto ground = engineInstance.CreatePlain(0.f, 1.f, 0.f, 0.f);
	pxScene->addActor(*ground);

	// Net
	auto net = engineInstance.CreateBox2DStatic(PxVec2{ 0, 2.5 }, 0.5, 2.5);
	pxScene->addActor(*net);
	
	// Shuttlecock
	PxVec2 location(-3, 10);
	PxVec2 velocity(2, 5);
	auto shuttlecock = make_shared<ShuttlecockNew>(location, velocity);

	auto rigidbody = engineInstance.CreateSphere2D(location, velocity, shuttlecock->GetRadius());
	PxRigidBodyExt::updateMassAndInertia(*rigidbody, 10.0f);
	pxScene->addActor(*rigidbody);

	shuttlecock->SetRigidbody(*rigidbody);

	gameObjects.push_back(shuttlecock);

	// LinkingContext에 등록
	replicationManager.linkingContext.AddGameObject(shuttlecock);
}

void Level::ClearLevel()
{
	RemoveAllGameObjects();

	_pendingSerializationQueue = queue<shared_ptr<GameObjectNew>>{};

	replicationManager.linkingContext.Clear();
}

void Level::Release()
{
	ClearLevel();
	PX_RELEASE(pxScene);
}

void Level::StepPhysics()
{
	pxScene->lockWrite();
	pxScene->simulate(Constant::PHYSX_FIXED_UPDATE_TIMESTEP);
	pxScene->fetchResults(true);
	pxScene->unlockWrite();
}

void Level::RemoveAllGameObjects()
{
	for (int idx = static_cast<int>(gameObjects.size()) - 1; idx >= 0; --idx)
		RemoveGameObject(idx);
}

void Level::RemoveGameObject(size_t idx)
{
	auto& go = gameObjects[idx];

	if (hasFlag(go->replicationFlag, ReplicationFlag::DF_ALL) == false)
		_pendingSerializationQueue.push(go);

	go->SetDirtyFlag(ReplicationFlag::DF_DELETE);

	auto& engineInstance = EngineNew::GetInstance();
	Remove(go->GetActor());

	swap(go, gameObjects.back());
	gameObjects.pop_back();
}

void Level::Remove(PxActor* actor)
{
	if (actor == nullptr || actor->isReleasable() == false)
		return;

	pxScene->lockWrite();
	pxScene->removeActor(*actor);
	pxScene->unlockWrite();

	actor->release(); // 메모리 해제
}

void Level::Update()
{
}

void Level::FixedUpdate()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - lastFixedUpdateTime;

	if (elapsedTime.count() < Constant::FIXED_UPDATE_TIMESTEP)
		return;

	lastFixedUpdateTime = currentTime;

	const local_time<system_clock::duration> now = zoned_time{ current_zone(), currentTime }.get_local_time();
	cout << "[" << now << "] FixedUpdate" << endl;

	// 아래 코드가 안정성을 보장하는지 검증 필요
	// ex) _gameObjects의 복사 중 _gameObjects의 요소의 추가/변경/삭제가 발생한다면?
	auto gameObjectsCopied = gameObjects;
	for (auto& gameObject : gameObjectsCopied)
	{
		auto& engineInstance = EngineNew::GetInstance();

		pxScene->lockRead();
		bool isChanged = gameObject->FixedUpdate();
		pxScene->unlockRead();

		if (isChanged == false)
			continue;

		ReplicationFlag& replicationFlag = gameObject->replicationFlag;

		if (hasFlag(replicationFlag, (ReplicationFlag::DF_UPDATE | ReplicationFlag::DF_DELETE)))
			continue;

		gameObject->SetDirtyFlag(ReplicationFlag::DF_UPDATE);
		_pendingSerializationQueue.push(gameObject);
	}
}

void Level::WriteWorldStateToStream()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - lastPacketUpdateTime;

	if (elapsedTime.count() < Constant::PACKET_PERIOD)
		return;

	lastPacketUpdateTime = currentTime;

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

int Level::WriteByReplication(OutputMemoryBitStream& outStream)
{
	int pendingSerializationCount = static_cast<int>(_pendingSerializationQueue.size());

	for (int i = 0; i < pendingSerializationCount; ++i) {
		shared_ptr<GameObjectNew> go = _pendingSerializationQueue.front();
		_pendingSerializationQueue.pop();

		ReplicationFlag& replicationFlag = go->replicationFlag;

		if (hasFlag(replicationFlag, ReplicationFlag::DF_DELETE)) {
			cout << "DF_DELETE" << endl;
			replicationManager.ReplicateDelete(outStream, go);
			replicationManager.linkingContext.RemoveGameObject(go);
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
