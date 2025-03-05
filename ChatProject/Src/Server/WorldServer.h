#pragma once

class GameObject;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	
	void InitWorld();
	
	void FixedUpdate();

private:
	WorldServer() = default;
	~WorldServer() {}

	system_clock::time_point _lastFixedUpdateTime;
	system_clock::time_point _lastPacketUpdateTime;

	std::vector<shared_ptr<GameObject>> _gameObjects;
};
