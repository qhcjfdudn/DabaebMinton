#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "OutputMemoryBitStream.h"
#include "Engine.h"

Shuttlecock::Shuttlecock(PxVec2 location, PxVec2 velocity) :
	GameObject(location, velocity)
{
}

float Shuttlecock::GetRadius()
{
	return _radius;
}

bool Shuttlecock::FixedUpdate()
{
	SetCurrentTransform();
	cout << "Shuttlecock FixedUpdate" << endl;
	cout << "Position: " << _location.x << ", " << _location.y << endl;

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
