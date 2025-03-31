#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"
#include "OutputMemoryBitStream.h"

GameObject::GameObject(PxVec2 position, PxVec2 velocity) :
	_location(position), _velocity(velocity), _rigidbody(nullptr) {}

GameObject::~GameObject()
{
	if (_rigidbody != nullptr)	// memory free needed but runtime error
	{
		//_rigidbody->release();
	}
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_velocity = velocity;
}

bool GameObject::FixedUpdate()
{
	SetCurrentTransform();

	return true;
}

void GameObject::SetCurrentTransform()
{
	PxVec3 curLocation{ _rigidbody->getGlobalPose().p };
	_location = PxVec2{ curLocation.x, curLocation.y };

	PxVec3 curVelocity{ _rigidbody->getLinearVelocity() };
	_velocity = PxVec2{ curVelocity.x, curVelocity.y };

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
