#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"

GameObject::GameObject(Vector2 position, Vector2 velocity) :
	_position(position), _velocity(velocity), _networkId(0)
{
}

void GameObject::FixedUpdate()
{
	MoveNextPosition();
}

void GameObject::SetVelocity(Vector2 velocity)
{
	_velocity = velocity;
}

void GameObject::MoveNextPosition()
{
	_position += _velocity * Constant::FIXED_UPDATE_TIMESTEP;
}
