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

bool Shuttlecock::FixedUpdate()
{
	PxVec3 curPos{ _rigidbody->getGlobalPose().p };
	PxVec2 curPos2D{ curPos.x, curPos.y };

	if ((curPos2D - _position).magnitude() > 1e-4f)
	{
		GameObject::FixedUpdate();
		
		MarkDirtyState(_networkId, static_cast<uint8_t>(ReplicationState::RS_Position));
		MarkDirtyState(_networkId, static_cast<uint8_t>(ReplicationState::RS_Velocity));
	}

	return true;
}
