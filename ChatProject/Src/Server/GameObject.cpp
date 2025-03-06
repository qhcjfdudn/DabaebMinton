#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"

GameObject::GameObject(Vector2 position, Vector2 velocity) :
	_position(position), _velocity(velocity), _networkId(0)
{
}

NetworkId_t GameObject::GetNetworkId()
{
	return _networkId;
}

void GameObject::SetVelocity(Vector2 velocity)
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
	return 0;
}

void GameObject::Write(OutputMemoryBitStream& inStream)
{

}
