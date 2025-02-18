#include "ServerPCH.h"
#include "GameObject.h"

#include "Constant.h"

GameObject::GameObject(Vector2 position) : _position(position), _velocity()
{
}

void GameObject::SetVelocity(Vector2 velocity)
{
	_velocity = velocity;
}

void GameObject::MoveNextPosition()
{
	_position = _position + _velocity * Constant::deltaTime;
}
