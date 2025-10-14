#include "ServerPCH.h"
#include "Level.h"

#include "PhysicsEngine.h"
#include "Constant.h"

#include "BadmintonBottom.h"
#include "BadmintonNet.h"

#include "Shuttlecock.h"
#include "Player.h"

Level::Level()
{
	auto& physicsEngine = PhysicsEngine::GetInstance();
	
	PxSceneDesc sceneDesc(physicsEngine.GetTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physicsEngine.GetCpuDispatcher();
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	
	_pxScene = physicsEngine.CreateScene(sceneDesc);

	if (_pxScene == nullptr)
	{
		cout << "PxScene 생성 실패" << endl;
		return;
	}

	PxPvdSceneClient* pvdClient = _pxScene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	_lastFixedUpdateTime = system_clock::now();
}

Level::~Level()
{
	auto& physicsEngine = PhysicsEngine::GetInstance();
	physicsEngine.Release(_pxScene);
}

void Level::InitLevel()
{
	auto& physicsEngine = PhysicsEngine::GetInstance();

	// Replication 불필요한 static GameObjects
	auto bottom = make_shared<BadmintonBottom>(PxVec2{ 0, 0.5f });
	_pxScene->addActor(*bottom->GetRigidbody());
	_staticGameObjects.push_back(bottom);

	auto net = make_shared<BadmintonNet>(PxVec2{ 0, 1.25f });
	_pxScene->addActor(*net->GetRigidbody());
	_staticGameObjects.push_back(net);
	
	// Replication 필요한 Dynamic GameObjects
	auto shuttlecock = make_shared<Shuttlecock>(PxVec2{ -2, 6.15f }, PxVec2{ 2, 1 });
	_pxScene->addActor(*shuttlecock->GetRigidbody());
	_gameObjects.push_back(shuttlecock);

	// Player
	auto player1 = make_shared<Player>(PxVec2{ -6, 5 });
	_pxScene->addActor(*player1->GetRigidbody());
	_gameObjects.push_back(player1);
}

void Level::ClearLevel()
{
	RemoveAllGameObjects();
	RemoveAllStaticGameObjects();
}

void Level::Release()
{
	ClearLevel();
}

void Level::RemoveAllGameObjects()
{
	for (int idx = static_cast<int>(_gameObjects.size()) - 1; idx >= 0; --idx)
		RemoveGameObject(idx);
}

void Level::RemoveGameObject(size_t idx)
{
	auto& go = _gameObjects[idx];

	auto& engineInstance = PhysicsEngine::GetInstance();
	Remove(go->GetRigidbody());

	swap(go, _gameObjects.back());
	_gameObjects.pop_back();
}

void Level::Remove(PxActor* actor)
{
	if (actor == nullptr || actor->isReleasable() == false)
		return;

	_pxScene->lockWrite();
	_pxScene->removeActor(*actor);
	_pxScene->unlockWrite();
}

void Level::RemoveAllStaticGameObjects()
{
	for (int idx = static_cast<int>(_staticGameObjects.size()) - 1; idx >= 0; --idx)
	{
		auto& go = _staticGameObjects[idx];
		PxActor* actor = go->GetRigidbody();
		if (actor == nullptr || actor->isReleasable() == false)
			continue;

		_pxScene->lockWrite();
		_pxScene->removeActor(*actor);
		_pxScene->unlockWrite();

		swap(go, _staticGameObjects.back());
		_staticGameObjects.pop_back();
	}
}

bool Level::HasElapsedFixedUpdateInterval()
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _lastFixedUpdateTime;

	return elapsedTime.count() >= Constant::FIXED_UPDATE_TIMESTEP;
}

void Level::SetLastFixedUpdateTimeToNow()
{
	_lastFixedUpdateTime = system_clock::now();
}

void Level::FixedUpdate()
{
	const local_time<system_clock::duration> now = zoned_time{ current_zone(), system_clock::now() }.get_local_time();
	spdlog::info("[Level::FixedUpdate] called.");

	// 아래 코드가 안정성을 보장하는지 검증 필요
	// ex) _gameObjects의 복사 중 _gameObjects의 요소의 추가/변경/삭제가 발생한다면?
	auto gameObjectsCopied = _gameObjects;
	for (auto& gameObject : gameObjectsCopied)
	{
		_pxScene->lockRead();
		bool isChanged = gameObject->FixedUpdate();
		_pxScene->unlockRead();
	}
}
