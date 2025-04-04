#pragma once

class OutputMemoryBitStream;

enum class ReplicationFlag {
	DF_NONE = 0,
	DF_CREATE = 1,
	DF_UPDATE = 2,
	DF_DELETE = 4,
	DF_ALL = DF_CREATE | DF_UPDATE | DF_DELETE
};
ReplicationFlag operator| (ReplicationFlag lhs, ReplicationFlag rhs);
ReplicationFlag& operator|= (ReplicationFlag& lhs, ReplicationFlag rhs);
ReplicationFlag operator& (ReplicationFlag lhs, ReplicationFlag rhs);

bool hasFlag(ReplicationFlag flags, ReplicationFlag flag);

class GameObject
{
public:
	ReplicationFlag replicationFlag = ReplicationFlag::DF_NONE;

	GameObject(PxVec2 location, PxVec2 velocity);
	virtual ~GameObject();

	void SetDirtyFlag(ReplicationFlag flag);

	void SetVelocity(PxVec2 velocity);
	
	virtual bool FixedUpdate();
	void SetCurrentTransform();

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);


protected:
	PxVec2 _location;
	PxVec2 _velocity;

	PxRigidDynamic* _rigidbody;
};
