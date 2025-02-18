#include "ServerPCH.h"
#include "Shuttlecock.h"

Shuttlecock::Shuttlecock(Vector2 position) : 
	GameObject(position), _radius(0)
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

void Shuttlecock::Update()
{
	
}

