#pragma once

#include <PxPhysicsAPI.h>

#include "Observer.h"

using namespace physx;

class Engine
{
public:
	static Engine& GetInstance();
	bool isRunning = true;

	void TurnOff();

	void initPhysics();
	void cleanupPhysics();
	void stepPhysics();

	void CreatePlain(float nx, float ny, float nz, float distance);

	PxRigidDynamic* createDynamic(
		const PxTransform& t,
		const PxGeometry& geometry,
		const PxVec3& velocity = PxVec3(0));

	void createStack(
		const PxTransform& t, 
		PxU32 size, 
		PxReal halfExtent);

private:
	Engine() = default;
	~Engine() {}

	PxDefaultAllocator		pxAllocator{};
	PxDefaultErrorCallback	pxErrorCallback{};
	PxFoundation*			pxFoundation = nullptr;
	PxPhysics*				pxPhysics = nullptr;
	PxDefaultCpuDispatcher*	pxDispatcher = nullptr;
	PxScene*				pxScene = nullptr;
	PxMaterial*				pxMaterial = nullptr;
	PxPvd*					pxPvd = nullptr;

	PxReal stackZ = 10.0f;

	Observer observer;
};

