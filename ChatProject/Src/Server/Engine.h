#pragma once

#include "Observer.h"

class Engine
{
public:
	static Engine& GetInstance();
	bool isRunning = true;

	void TurnOff();

	void initPhysics();
	void cleanupPhysics();

	const PxTolerancesScale& GetTolerancesScale() const;
	PxCpuDispatcher* GetCpuDispatcher();
	PxScene* CreateScene(PxSceneDesc sceneDesc);

	PxRigidStatic* CreatePlain(float nx, float ny, float nz, float distance);

	PxRigidDynamic * CreateBox(const PxTransform& tp, float halfExtentX, float halfExtentY, float halfExtentZ);
	PxRigidDynamic * CreateBox2D(const PxVec2& location, float halfExtentX, float halfExtentY);
	PxRigidStatic* CreateBox2DStatic(const PxVec2& location, float halfExtentX, float halfExtentY);

	PxRigidDynamic * CreateSphere2D(const PxVec2& location, const PxVec2& velocity, float halfExtentRadius);

	PxRigidDynamic * createDynamic(const PxTransform& t,
		const PxGeometry& geometry,
		const PxVec3& velocity = PxVec3(0));

private:
	Engine() = default;
	~Engine() {}

	PxDefaultAllocator		pxAllocator{};
	PxDefaultErrorCallback	pxErrorCallback{};
	PxFoundation* pxFoundation = nullptr;
	PxPhysics* pxPhysics = nullptr;
	PxDefaultCpuDispatcher* pxDispatcher = nullptr;
	PxMaterial* pxMaterial = nullptr;
	PxPvd* pxPvd = nullptr;

	PxReal stackZ = 10.0f;

	Observer observer;
};

