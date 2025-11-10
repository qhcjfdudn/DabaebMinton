#include "ServerPCH.h"
#include "GameObject.h"

#include "OutputMemoryBitStream.h"
#include "BitSizeCounter.h"

GameObject::GameObject(PxVec2 location, PxVec2 velocity) :
	_position(location), _velocity(velocity) {
}

GameObject::~GameObject()
{
	if (_rigidbody && _rigidbody->isReleasable())
		_rigidbody->release();
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_rigidbody->setLinearVelocity(PxVec3{ velocity.x, velocity.y, 0.0f });
}

PxActor* GameObject::GetRigidbody() const
{
	return _rigidbody;
}

void GameObject::SetRigidbody(PxRigidDynamic& rigidbody)
{
	_rigidbody = &rigidbody;
}

bool GameObject::FixedUpdate()
{
	SetCurrentTransform();

	return true;
}

void GameObject::SetCurrentTransform()
{
	PxVec3 curPosition{ _rigidbody->getGlobalPose().p };
	PxVec3 curVelocity{ _rigidbody->getLinearVelocity() };

	_position = PxVec2{ curPosition.x, curPosition.y };
	_velocity = PxVec2{ curVelocity.x, curVelocity.y };
}

uint8_t GameObject::Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const
{
	uint8_t writtenState = 0;

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Position))
	{
		inStream.Write(true);
		inStream.Write(_position);
		
		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Position);
	}
	else
	{
		inStream.Write(false);
	}

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Velocity))
	{
		inStream.Write(true);
		inStream.Write(_velocity);
		
		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Velocity);
	}
	else
	{
		inStream.Write(false);
	}

	return writtenState;
}
size_t GameObject::CountWriteBitSize(const uint8_t inDirtyState) const
{
	size_t totalBits = 0;

	totalBits += 1;
	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Position))
	{
		totalBits += BitSizeCounter::Count(_position);
	}

	totalBits += 1;
	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Velocity))
	{
		totalBits += BitSizeCounter::Count(_velocity);
	}

	return totalBits;
}
