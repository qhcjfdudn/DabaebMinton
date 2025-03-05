#include "ServerPCH.h"
#include "Shuttlecock.h"

Shuttlecock::Shuttlecock(Vector2 position, Vector2 velocity) :
	GameObject(position, velocity)
{
}

void Shuttlecock::setRadius(int radius)
{
	_radius = radius;
}

int Shuttlecock::getRadius()
{
	return _radius;
}

void Shuttlecock::FixedUpdate()
{
	MoveNextPosition();
	cout << "Shuttlecock FixedUpdate" << endl;
	cout << "Position: " << _position._x << ", " << _position._y << endl;
}

