#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "OutputMemoryBitStream.h"
#include "Engine.h"

Shuttlecock::Shuttlecock(PxVec2 location, PxVec2 velocity) :
	GameObject(location, velocity)
{
	auto& engineInstance = Engine::GetInstance();

	_rigidbody = engineInstance.CreateSphere2D(location, velocity, _radius);
	PxRigidBodyExt::updateMassAndInertia(*_rigidbody, 10.0f);
}

Shuttlecock::~Shuttlecock()
{
	_rigidbody->release();
}

float Shuttlecock::GetRadius() const
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
