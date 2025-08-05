#include "ServerPCH.h"
#include "Engine.h"

#include "Constant.h"

Engine& Engine::GetInstance() {
	static Engine instance;
	return instance;
}

void Engine::TurnOff()
{
	isRunning = false;
	observer.notify(ObserverEvent::EngineOff);
}

void Engine::initPhysics()
{
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);

	pxPvd = PxCreatePvd(*pxFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, PxTolerancesScale(), true, pxPvd);
	pxDispatcher = PxDefaultCpuDispatcherCreate(2);

	cout << "initPhysics done." << endl;
}

void Engine::cleanupPhysics()
{
	PX_RELEASE(pxDispatcher);
	PX_RELEASE(pxPhysics);
	if (pxPvd)
	{
		PxPvdTransport* transport = pxPvd->getTransport();
		PX_RELEASE(pxPvd);
		PX_RELEASE(transport);
	}
	PX_RELEASE(pxFoundation);

	cout << "cleanupPhysics done." << endl;
}

const PxTolerancesScale& Engine::GetTolerancesScale() const
{
	return pxPhysics->getTolerancesScale();
}

PxCpuDispatcher* Engine::GetCpuDispatcher()
{
	return pxDispatcher;
}

PxScene* Engine::CreateScene(PxSceneDesc sceneDesc)
{
	return pxPhysics->createScene(sceneDesc);
}

PxRigidStatic* Engine::CreatePlain(float nx, float ny, float nz, float distance)
{
	pxMaterial = pxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	return PxCreatePlane(*pxPhysics, PxPlane(nx, ny, nz, distance), *pxMaterial);
}

PxRigidDynamic * Engine::CreateBox(const PxTransform& tp, float halfExtentX, float halfExtentY, float halfExtentZ)
{
	PxRigidDynamic* body = pxPhysics->createRigidDynamic(tp);

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, halfExtentZ), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

	return body;
}

PxRigidDynamic * Engine::CreateBox2D(const PxVec2& location, float halfExtentX, float halfExtentY)
{
	PxRigidDynamic* body = pxPhysics->createRigidDynamic(PxTransform{ location.x, location.y, 0 });

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, 0.1f), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);

	return body;
}

PxRigidStatic* Engine::CreateBox2DStatic(const PxVec2& location, float halfExtentX, float halfExtentY)
{
	PxRigidStatic* body = pxPhysics->createRigidStatic(PxTransform{ location.x, location.y, 0 });

	PxShape* shape = pxPhysics->createShape(PxBoxGeometry(halfExtentX, halfExtentY, 0.1f), *pxMaterial);
	body->attachShape(*shape);
	shape->release();

	return body;
}

PxRigidDynamic * Engine::CreateSphere2D(const PxVec2& location, const PxVec2& velocity, float halfExtentRadius)
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

PxRigidDynamic * Engine::createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity)
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*pxPhysics, t, geometry, *pxMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);

	return dynamic;
}
