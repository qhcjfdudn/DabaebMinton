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

	spdlog::info("[PhysicsEngine::InitPhysics] InitPhysics done.");
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

	spdlog::info("[PhysicsEngine::CleanupPhysics] CleanupPhysics done.");
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
		spdlog::error("[PhysicsEngine::CreateSphere2D] Shuttlecock(): CreateSphere2D error.");
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

PxRigidDynamic* PhysicsEngine::CreateCapsule2D(const PxVec2& location, float radius, float halfHeight)
{
	PxRigidDynamic* rb = pxPhysics->createRigidDynamic(PxTransform{ location.x, location.y, 0 });
	PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0, 0, 1)));
	PxShape* aCapsuleShape = PxRigidActorExt::createExclusiveShape(*rb,
		PxCapsuleGeometry(radius, halfHeight), *pxMaterial);
	aCapsuleShape->setLocalPose(relativePose);

	rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	rb->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);

	return rb;
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
}

PhysicsEngineRunningState PhysicsEngine::GetEngineRunningState()
{
	return _engineRunningState;
}
