#pragma once

#include "ReplicationFlag.h"

class OutputMemoryBitStream;

class GameObject
{
public:
	ReplicationFlag replicationFlag = ReplicationFlag::DF_NONE;

	GameObject(PxVec2 location, PxVec2 velocity);
	virtual ~GameObject() {}

	void SetDirtyFlag(ReplicationFlag flag);

	void SetVelocity(PxVec2 velocity);
	void SetRigidbody(PxRigidDynamic& rigidbody);

	virtual bool FixedUpdate();
	void SetCurrentTransform();

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);

	PxActor* GetActor() const;

protected:
	PxVec2 _location;
	PxVec2 _velocity;

	PxRigidDynamic* _rigidbody = nullptr;
};

