#pragma once

class GameObject;
class LinkingContext;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	
	void InitLevel();

	void Update();
	void FixedUpdate();
	void WriteWorldStateToStream();

private:
	WorldServer();
	~WorldServer() {}

	system_clock::time_point _lastFixedUpdateTime;
	system_clock::time_point _lastPacketUpdateTime;

	std::vector<shared_ptr<GameObject>> _gameObjects;

	unordered_set<NetworkId_t> _updatedObjectNetworkIds;
	queue<NetworkId_t> _pendingSerializationQueue;

	LinkingContext& _linkingContext;
};
