#include "ServerPCH.h"
#include "StaticGameObject.h"

#include "OutputMemoryBitStream.h"

StaticGameObject::StaticGameObject(PxVec2 location) :
	_location(location) {}

StaticGameObject::~StaticGameObject()
{
	if (_rigidbody && _rigidbody->isReleasable())
		_rigidbody->release();
}

void StaticGameObject::SetRigidbody(PxRigidStatic& rigidbody)
{
	_rigidbody = &rigidbody;
}

unsigned int StaticGameObject::GetClassId()
{
	return 'SGOJ';
}

void StaticGameObject::Write(OutputMemoryBitStream& inStream)
{
	inStream.Write(_location);
}

PxActor* StaticGameObject::GetRigidbody() const
{
	return _rigidbody;
}
