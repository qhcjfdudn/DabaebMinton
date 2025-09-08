#include "ServerPCH.h"
#include "PhysicsEngine.h"

#include "Constant.h"

PhysicsEngine& PhysicsEngine::GetInstance() {
	static PhysicsEngine instance;
	return instance;
}

void PhysicsEngine::InitPhysics()
{
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);

	pxPvd = PxCreatePvd(*pxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, pxPvd);
	pxDispatcher = PxDefaultCpuDispatcherCreate(2);

	_engineRunningState = PhysicsEngineRunningState::Running;

	cout << "InitPhysics done." << endl;
}

void PhysicsEngine::CleanupPhysics()
{
	// 관리하는 모든 객체를 여기서 반납해버려도 될까?

	PX_RELEASE(pxDispatcher);
	PX_RELEASE(pxPhysics);
	if (pxPvd)
	{
		PxPvdTransport* transport = pxPvd->getTransport();
		PX_RELEASE(pxPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(pxFoundation);

	_engineRunningState = PhysicsEngineRunningState::TurnedOff;

	cout << "CleanupPhysics done." << endl;
}

const PxTolerancesScale& PhysicsEngine::GetTolerancesScale() const
{
	return pxPhysics->getTolerancesScale();
}

PxCpuDispatcher* PhysicsEngine::GetCpuDispatcher()
{
	return pxDispatcher;
}

PxScene* PhysicsEngine::CreateScene(PxSceneDesc sceneDesc)
{
	PxScene* pxScene = pxPhysics->createScene(sceneDesc);
	
	std::lock_guard lk(scenesMutex);
	scenes.push_back(pxScene);
	_lastPhysXFixedUpdateTimeArray.push_back(system_clock::now());
	
	return pxScene;
}

void PhysicsEngine::Release(PxScene* scene)
{
	std::lock_guard lk(scenesMutex);
	auto iter = std::find(scenes.begin(), scenes.end(), scene);
	if (iter == scenes.end())
		return;

	int idx = static_cast<int>(iter - scenes.begin());
	std::swap(scenes[idx], scenes[scenes.size() - 1]);
	scenes.pop_back();

	PX_RELEASE(scene);
}

void PhysicsEngine::ReleaseEveryScene()
{
	std::lock_guard lk(scenesMutex);
	for (auto scene : scenes)
		PX_RELEASE(scene);

	scenes.clear();
}

PxRigidStatic* PhysicsEngine::CreatePlain(float nx, float ny, float nz, float distance)
{
	pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	return PxCreatePlane(*pxPhysics, PxPlane(nx, ny, nz, distance), *pxMaterial);
}

PxRigidDynamic * PhysicsEngine::CreateBox(const PxTransform& tp, float halfExtentX, float halfExtentY, float halfExtentZ)
{
	PxRigidDynamic* body = pxPhysics->createRigidDynamic(tp);

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, halfExtentZ), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

	return body;
}

PxRigidDynamic * PhysicsEngine::CreateBox2D(const PxVec2& location, float halfExtentX, float halfExtentY)
{
	PxRigidDynamic* body = pxPhysics->createRigidDynamic(PxTransform{ location.x, location.y, 0 });

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, 0.1f), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

	return body;
}

PxRigidStatic* PhysicsEngine::CreateBox2DStatic(const PxVec2& location, float halfExtentX, float halfExtentY)
{
	PxRigidStatic* body = pxPhysics->createRigidStatic(PxTransform{ location.x, location.y, 0 });

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, 0.1f), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	return body;
}

PxRigidDynamic * PhysicsEngine::CreateSphere2D(const PxVec2& location, const PxVec2& velocity, float halfExtentRadius)
{
	PxRigidDynamic* body = pxPhysics->createRigidDynamic(PxTransform{ location.x, location.y, 0 });

	if (body == nullptr)
	{
		cout << "Shuttlecock(): CreateSphere2D error" << endl;
		return nullptr;
	}

	body->setLinearVelocity(PxVec3{ velocity.x, velocity.y, 0.f });
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	body->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);

	PxShape* shape = pxPhysics->createShape(PxSphereGeometry(halfExtentRadius), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	return body;
}

PxRigidDynamic * PhysicsEngine::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*pxPhysics, t, geometry, *pxMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);

	return dynamic;
}

void PhysicsEngine::StepPhysics(PxScene* scene, PxReal elapsedTime)
{
	scene->lockWrite();
	scene->simulate(elapsedTime);
	scene->fetchResults(true);
	scene->unlockWrite();

	cout << "[" << system_clock::now() << " PhysicsEngine::StepPhysics] update scene." << endl;
}

void PhysicsEngine::SetLastUpdateTimeToNow(system_clock::time_point& lastUpdateTime)
{
	lastUpdateTime = system_clock::now();
}

bool PhysicsEngine::StepPhysicsIfHasElapsedPhysicsFixedUpdateInterval(PxScene* scene, system_clock::time_point& lastUpdateTime)
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - lastUpdateTime;

	if (elapsedTime.count() >= Constant::PHYSX_FIXED_UPDATE_TIMESTEP)
	{
		StepPhysics(scene, static_cast<PxReal>(elapsedTime.count()));
		lastUpdateTime = currentTime;

		return true;
	}

	return false;
}

PhysicsEngineRunningState PhysicsEngine::GetEngineRunningState()
{
	return _engineRunningState;
}
