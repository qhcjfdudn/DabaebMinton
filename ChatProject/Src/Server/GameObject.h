#pragma once

class OutputMemoryBitStream;

class GameObject
{
public:
	GameObject(Vector2 position, Vector2 velocity);

	virtual ~GameObject() {}

	void SetVelocity(Vector2 velocity);
	
	virtual bool FixedUpdate();
	void MoveNextPosition();

	virtual unsigned int GetClassId();
	virtual void Write(OutputMemoryBitStream& inStream);


protected:
	Vector2 _position;
	Vector2 _velocity;
};

