#pragma once

class GameObject
{
public:
	GameObject();
	virtual ~GameObject() {}

	virtual void Update() {}

private:
	unsigned int _gameObjectId;
	Vector2 _position;
	Vector2 _velocity;
};

