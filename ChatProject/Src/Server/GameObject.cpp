#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"
#include "OutputMemoryBitStream.h"

GameObject::GameObject(PxVec2 position, PxVec2 velocity) :
	_position(position), _velocity(velocity)
{
}

void GameObject::SetVelocity(PxVec2 velocity)
{
	_velocity = velocity;
}

bool GameObject::FixedUpdate()
{
	MoveNextPosition();

	return true;
}

void GameObject::MoveNextPosition()
{
	_position += _velocity * Constant::FIXED_UPDATE_TIMESTEP;
}

unsigned int GameObject::GetClassId()
{
	return 'GMOJ';
}

void GameObject::Write(OutputMemoryBitStream& inStream)
{
	inStream.Write(_position);
	inStream.Write(_velocity);
}
