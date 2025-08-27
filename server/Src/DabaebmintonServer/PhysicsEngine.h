#pragma once

class PhysicsEngine
{
public:
	static PhysicsEngine& GetInstance();
	
	void InitPhysics();
	void CleanupPhysics();

	const PxTolerancesScale& GetTolerancesScale() const;
	PxCpuDispatcher* GetCpuDispatcher();
	PxScene* CreateScene(PxSceneDesc sceneDesc);

	PxRigidStatic* CreatePlain(float nx, float ny, float nz, float distance);

	PxRigidDynamic* CreateBox(const PxTransform& tp, float halfExtentX, float halfExtentY, float halfExtentZ);
	PxRigidDynamic* CreateBox2D(const PxVec2& location, float halfExtentX, float halfExtentY);
	PxRigidStatic* CreateBox2DStatic(const PxVec2& location, float halfExtentX, float halfExtentY);

	PxRigidDynamic* CreateSphere2D(const PxVec2& location, const PxVec2& velocity, float halfExtentRadius);

	PxRigidDynamic* createDynamic(const PxTransform& t,
		const PxGeometry& geometry,
		const PxVec3& velocity = PxVec3(0));

	void StepPhysicsEveryScene();

	system_clock::time_point lastPhysxFixedUpdateTime;

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

	vector<PxScene*> scenes{};

	PxReal stackZ = 10.0f;
};

