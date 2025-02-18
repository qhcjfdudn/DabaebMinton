#pragma once

class GameObject;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	void Update();

private:
	WorldServer() = default;
	~WorldServer() {}

	void InitWorld();

	std::vector<shared_ptr<GameObject>> _gameObjects;
};
