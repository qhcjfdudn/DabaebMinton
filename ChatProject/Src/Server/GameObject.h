#pragma once

class OutputMemoryBitStream;

class GameObject
{
public:
	GameObject(PxVec2 position, PxVec2 velocity);

	virtual ~GameObject() {}

	void SetVelocity(PxVec2 velocity);
	
	virtual bool FixedUpdate();
	void MoveNextPosition();

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);


protected:
	PxVec2 _position;
	PxVec2 _velocity;
};

