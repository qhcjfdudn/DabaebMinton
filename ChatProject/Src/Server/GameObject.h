#pragma once

class GameObject
{
public:
	GameObject(Vector2 position);
	virtual ~GameObject() {}

	virtual void FixedUpdate();

	void SetVelocity(Vector2 velocity);
	void MoveNextPosition();

protected:
	Vector2 _position;

private:
	unsigned int _networkId;
	Vector2 _velocity;
};

