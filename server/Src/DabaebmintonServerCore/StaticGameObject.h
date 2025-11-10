#pragma once

class OutputMemoryBitStream;

class StaticGameObject
{
public:

	StaticGameObject(PxVec2 location);
	virtual ~StaticGameObject();

	void SetRigidbody(PxRigidStatic& rigidbody);

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);

	PxActor* GetRigidbody() const;

protected:
	PxVec2 _position;
	PxVec2 _velocity;

	PxRigidStatic* _rigidbody = nullptr;
};

