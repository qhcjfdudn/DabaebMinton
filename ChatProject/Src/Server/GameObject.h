#pragma once

class OutputMemoryBitStream;

enum class DirtyFlag {
	DF_NONE = 0,
	DF_CREATE = 1,
	DF_UPDATE = 2,
	DF_DELETE = 4,
	DF_ALL = DF_CREATE | DF_UPDATE | DF_DELETE
};
DirtyFlag operator| (DirtyFlag lhs, DirtyFlag rhs);
DirtyFlag& operator|= (DirtyFlag& lhs, DirtyFlag rhs);
DirtyFlag operator& (DirtyFlag lhs, DirtyFlag rhs);

bool hasFlag(DirtyFlag flags, DirtyFlag flag);

class GameObject
{
public:
	DirtyFlag dirtyFlag = DirtyFlag::DF_NONE;

	GameObject(PxVec2 location, PxVec2 velocity);
	virtual ~GameObject();

	void SetDirtyFlag(DirtyFlag flag);

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
