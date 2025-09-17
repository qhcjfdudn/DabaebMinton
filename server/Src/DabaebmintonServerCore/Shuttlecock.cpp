#include "ServerPCH.h"
#include "Shuttlecock.h"

#include "OutputMemoryBitStream.h"
#include "PhysicsEngine.h"

Shuttlecock::Shuttlecock(PxVec2 location, PxVec2 velocity) :
	GameObject(location, velocity)
{
	auto& engineInstance = PhysicsEngine::GetInstance();

	_rigidbody = engineInstance.CreateSphere2D(location, velocity, _radius);
	PxRigidBodyExt::updateMassAndInertia(*_rigidbody, 10.0f);
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

void Shuttlecock::Write(OutputMemoryBitStream& inStream) const
{
	GameObject::Write(inStream);
}

size_t Shuttlecock::CountWriteBitSize(const uint8_t inState) const
{
	return GameObject::CountWriteBitSize(inState);
}
