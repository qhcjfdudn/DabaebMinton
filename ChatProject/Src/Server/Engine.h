#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

class Engine
{
public:
	static Engine& GetInstance();
	bool isRunning;

	void initPhysics(bool interactive);

	static PxRigidDynamic* createDynamic(
		const PxTransform& t,
		const PxGeometry& geometry,
		const PxVec3& velocity = PxVec3(0));

	static void createStack(
		const PxTransform& t, 
		PxU32 size, 
		PxReal halfExtent);

	void stepPhysics(bool /*interactive*/);

	void cleanupPhysics(bool /*interactive*/);

private:
	Engine();
	~Engine() {}

	static PxDefaultAllocator		gAllocator;
	static PxDefaultErrorCallback	gErrorCallback;
	static PxFoundation*			gFoundation;
	static PxPhysics*				gPhysics;
	static PxDefaultCpuDispatcher*	gDispatcher;
	static PxScene*					gScene;
	static PxMaterial*				gMaterial;
	static PxPvd*					gPvd;

	static PxReal stackZ;
};

