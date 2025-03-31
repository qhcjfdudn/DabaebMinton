#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"
#include "OutputMemoryBitStream.h"

GameObject::GameObject(PxVec2 position, PxVec2 velocity) :
	_location(position), _velocity(velocity)
{
	_rigidbody = nullptr;
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_velocity = velocity;
}

bool GameObject::FixedUpdate()
{
	SetCurrentLocation();

	return true;
}

void GameObject::SetCurrentLocation()
{
	PxVec3 curLocation{ _rigidbody->getGlobalPose().p };
	_location = PxVec2{ curLocation.x, curLocation.y };
}

unsigned int GameObject::GetClassId()
{
	return 'GMOJ';
}

void GameObject::Write(OutputMemoryBitStream& inStream)
{
	inStream.Write(_location);
	inStream.Write(_velocity);
}
