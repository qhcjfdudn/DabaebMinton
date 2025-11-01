#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "PhysicsEngine.h"
#include "OutputMemoryBitStream.h"

Shuttlecock::Shuttlecock(PxVec2 location, PxVec2 velocity) :
	GameObject(location, velocity)
{
	auto& physicsEngine = PhysicsEngine::GetInstance();

	_rigidbody = physicsEngine.CreateSphere2D(location, velocity, _radius);
	PxRigidBodyExt::updateMassAndInertia(*_rigidbody, 1.0f);

	_rigidbody->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
}

float Shuttlecock::GetRadius() const
{
	return _radius;
}

size_t Shuttlecock::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}
