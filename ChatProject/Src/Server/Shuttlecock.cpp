#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "OutputMemoryBitStream.h"
#include "Engine.h"

Shuttlecock::Shuttlecock(PxVec2 location, PxVec2 velocity) :
	GameObject(location, velocity)
{
	Engine& engineInstance = Engine::GetInstance();
	_rigidbody = engineInstance.CreateSphere2D(location, _radius);

	if (_rigidbody == nullptr)
	{
		cout << "cork: CreateSphere2D error" << endl;
		return;
	}

	_rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	_rigidbody->setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
}

void Shuttlecock::setRadius(float radius)
{
	_radius = radius;
}

float Shuttlecock::getRadius()
{
	return _radius;
}

bool Shuttlecock::FixedUpdate()
{
	SetCurrentLocation();
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
