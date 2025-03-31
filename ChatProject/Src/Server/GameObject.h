#pragma once

class OutputMemoryBitStream;

class GameObject
{
public:
	GameObject(PxVec2 location, PxVec2 velocity);

	virtual ~GameObject() {}

	void SetVelocity(PxVec2 velocity);
	
	virtual bool FixedUpdate();
	void SetCurrentLocation();

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);


protected:
	PxVec2 _location;
	PxVec2 _velocity;

	PxRigidDynamic* _rigidbody;
};

