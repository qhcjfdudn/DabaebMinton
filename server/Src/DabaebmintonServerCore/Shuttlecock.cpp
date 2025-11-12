#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "PhysicsEngine.h"
#include "OutputMemoryBitStream.h"

Shuttlecock::Shuttlecock(PxVec2 position, PxVec2 velocity) :
	GameObject(position, velocity)
{
	auto& physicsEngine = PhysicsEngine::GetInstance();

	_rigidbody = physicsEngine.CreateDefaultShuttlecock(_radius, position, velocity);
}

float Shuttlecock::GetRadius() const
{
	return _radius;
}

size_t Shuttlecock::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}
