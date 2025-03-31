#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "OutputMemoryBitStream.h"

Shuttlecock::Shuttlecock(PxVec2 position, PxVec2 velocity) :
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

bool Shuttlecock::FixedUpdate()
{
	MoveNextPosition();
	cout << "Shuttlecock FixedUpdate" << endl;
	cout << "Position: " << _position.x << ", " << _position.y << endl;

	return true;
}

unsigned int Shuttlecock::GetClassId()
{
	return 'STCK';
}

void Shuttlecock::Write(OutputMemoryBitStream& inStream)
{
	GameObject::Write(inStream);
}
