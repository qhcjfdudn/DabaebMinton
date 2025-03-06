#pragma once

class GameObject
{
public:
	GameObject(Vector2 position, Vector2 velocity);

	virtual ~GameObject() {}

	NetworkId_t GetNetworkId();
	void SetVelocity(Vector2 velocity);
	
	virtual bool FixedUpdate();
	void MoveNextPosition();


protected:
	Vector2 _position;

private:
	NetworkId_t _networkId;
	Vector2 _velocity;
};

