#include "ServerPCH.h"
#include "GameObject.h"

#include "OutputMemoryBitStream.h"
#include "BitSizeCounter.h"

GameObject::GameObject(PxVec2 location, PxVec2 velocity) :
	_location(location), _velocity(velocity) {
}

GameObject::~GameObject()
{
	if (_rigidbody && _rigidbody->isReleasable())
		_rigidbody->release();
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_velocity = velocity;
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
	// simulate 중에는 getGlobalPose()를 사용할 수 없다는 에러 메시지 발견
	// 물리 엔진에 접근해 값을 알아오고자 할 때는 lockRead()를 걸어야 한다.

	PxVec3 curLocation{ _rigidbody->getGlobalPose().p };
	PxVec3 curVelocity{ _rigidbody->getLinearVelocity() };

	_location = PxVec2{ curLocation.x, curLocation.y };
	_velocity = PxVec2{ curVelocity.x, curVelocity.y };
}

void GameObject::Write(OutputMemoryBitStream& inStream) const
{
	inStream.Write(_location);
	inStream.Write(_velocity);
}

uint8_t GameObject::Write(OutputMemoryBitStream& inStream, uint8_t inDirtyState) const
{
	uint8_t writtenState = 0;

	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Location))
	{
		inStream.Write(true);
		inStream.Write(_location);
		
		writtenState |= static_cast<uint8_t>(ReplicationState::RS_Location);
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
	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Location))
	{
		totalBits += BitSizeCounter::Count(_location);
	}

	totalBits += 1;
	if (inDirtyState & static_cast<uint8_t>(ReplicationState::RS_Velocity))
	{
		totalBits += BitSizeCounter::Count(_velocity);
	}

	return totalBits;
}
