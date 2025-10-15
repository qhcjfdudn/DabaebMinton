#pragma once

enum class PhysicsEngineRunningState {
	Initiating,
	Running,
	Exiting,
	TurnedOff
};

class PhysicsEngine
{
public:
	static PhysicsEngine& GetInstance();
	
	void InitPhysics();
	void CleanupPhysics();

	const PxTolerancesScale& GetTolerancesScale() const;
	PxCpuDispatcher* GetCpuDispatcher();
	PxScene* CreateScene(PxSceneDesc sceneDesc);
	void Release(PxScene* scene);
	void ReleaseEveryScene();

	PxRigidStatic* CreatePlain(float nx, float ny, float nz, float distance);

	PxRigidDynamic* CreateBox(const PxTransform& tp, float halfExtentX, float halfExtentY, float halfExtentZ);
	PxRigidDynamic* CreateBox2D(const PxVec2& location, float halfExtentX, float halfExtentY);
	PxRigidStatic* CreateBox2DStatic(const PxVec2& location, float halfExtentX, float halfExtentY);

	PxRigidDynamic* CreateSphere2D(const PxVec2& location, const PxVec2& velocity, float halfExtentRadius);

	PxRigidDynamic* CreateCapsule2D(const PxVec2& location, float radius, float halfHeight);

	PxRigidDynamic* createDynamic(const PxTransform& t,
		const PxGeometry& geometry,
		const PxVec3& velocity = PxVec3(0));

	void StepPhysics(PxScene* scene, PxReal elapsedTime);

	PhysicsEngineRunningState GetEngineRunningState();

	std::vector<PxScene*> scenes;
	std::mutex scenesMutex;

private:
	PhysicsEngine() = default;
	~PhysicsEngine() {}

	PxDefaultAllocator		pxAllocator{};
	PxDefaultErrorCallback	pxErrorCallback{};
	PxFoundation* pxFoundation = nullptr;
	PxPhysics* pxPhysics = nullptr;
	PxDefaultCpuDispatcher* pxDispatcher = nullptr;
	PxMaterial* pxMaterial = nullptr;
	PxPvd* pxPvd = nullptr;

	PhysicsEngineRunningState _engineRunningState{ PhysicsEngineRunningState::Initiating };

	PxReal stackZ = 10.0f;
};

