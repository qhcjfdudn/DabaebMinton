#include "ServerPCH.h"
#include "Level.h"

#include "PhysicsEngine.h"

#include "Constant.h"

#include "PacketQueue.h"
#include "OutputMemoryBitStream.h"
#include "GetRequiredBits.h"
#include "NetworkManagerServer.h"

#include "Shuttlecock.h"
#include "BadmintonBottom.h"
#include "BadmintonNet.h"

Level::Level()
{
	auto& engineInstance = PhysicsEngine::GetInstance();
	
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

	lastFixedUpdateTime = system_clock::now();
}

Level::~Level()
{
	PX_RELEASE(pxScene);
}

void Level::InitLevel()
{
	auto& engineInstance = PhysicsEngine::GetInstance();

	auto bottom = make_shared<BadmintonBottom>(PxVec2{ 0, 0 });
	pxScene->addActor(*bottom->GetRigidbody());
	staticGameObjects.push_back(bottom);

	// Net
	auto net = make_shared<BadmintonNet>(PxVec2{ 0, 2.5f });
	pxScene->addActor(*net->GetRigidbody());
	staticGameObjects.push_back(net);
	
	// Shuttlecock
	auto shuttlecock = make_shared<Shuttlecock>(PxVec2{ -3, 10 }, PxVec2{ 2, 5 });
	pxScene->addActor(*shuttlecock->GetRigidbody());
	gameObjects.push_back(shuttlecock);

	NetworkManagerServer::GetInstance().AddGameObjectForReplication(shuttlecock);
}

void Level::ClearLevel()
{
	RemoveAllGameObjects();
	RemoveAllStaticGameObjects();
}

void Level::Release()
{
	ClearLevel();
	PX_RELEASE(pxScene);
}

void Level::RemoveAllGameObjects()
{
	for (int idx = static_cast<int>(gameObjects.size()) - 1; idx >= 0; --idx)
		RemoveGameObject(idx);
}

void Level::RemoveGameObject(size_t idx)
{
	auto& go = gameObjects[idx];

	NetworkManagerServer::GetInstance().RemoveGameObjectForReplication(go);

	auto& engineInstance = PhysicsEngine::GetInstance();
	Remove(go->GetRigidbody());

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
}

void Level::RemoveAllStaticGameObjects()
{
	for (int idx = static_cast<int>(staticGameObjects.size()) - 1; idx >= 0; --idx)
	{
		auto& go = staticGameObjects[idx];
		PxActor* actor = go->GetRigidbody();
		if (actor == nullptr || actor->isReleasable() == false)
			continue;

		pxScene->lockWrite();
		pxScene->removeActor(*actor);
		pxScene->unlockWrite();

		swap(go, staticGameObjects.back());
		staticGameObjects.pop_back();
	}
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
		auto& engineInstance = PhysicsEngine::GetInstance();

		pxScene->lockRead();
		bool isChanged = gameObject->FixedUpdate();
		pxScene->unlockRead();
	}
}