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

	PxDefaultAllocator		gAllocator{};
	PxDefaultErrorCallback	gErrorCallback{};
	PxFoundation*			gFoundation = nullptr;
	PxPhysics*				gPhysics = nullptr;
	PxDefaultCpuDispatcher*	gDispatcher = nullptr;
	PxScene*				gScene = nullptr;
	PxMaterial*				gMaterial = nullptr;
	PxPvd*					gPvd = nullptr;

	PxReal stackZ = 10.0f;

	Observer observer;
};

