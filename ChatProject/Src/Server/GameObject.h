#pragma once

class GameObject
{
public:
	GameObject(Vector2 position);
	virtual ~GameObject() {}

	virtual void Update();

	void SetVelocity(Vector2 velocity);
	void MoveNextPosition();

private:
	unsigned int _networkId;
	Vector2 _position;
	Vector2 _velocity;
};

