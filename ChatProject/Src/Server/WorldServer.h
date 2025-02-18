#pragma once

class GameObject;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	
	void InitWorld();
	
	void Update();

private:
	WorldServer() = default;
	~WorldServer() {}

	std::vector<shared_ptr<GameObject>> _gameObjects;
};
