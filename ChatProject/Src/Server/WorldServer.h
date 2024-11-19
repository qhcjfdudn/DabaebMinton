#pragma once

class GameObject;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	void Update();

private:
	WorldServer() {}
	~WorldServer() {}

	std::vector<GameObject*> gameObjects;
	std::queue<GameObject*> updateQueue;
};
