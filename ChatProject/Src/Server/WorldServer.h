#pragma once

class GameObject;
class LinkingContext;

class WorldServer
{
public:
	static WorldServer& GetInstance();
	
	void InitLevel();

	void RemoveAllGameObjects();
	void RemoveGameObject(size_t idx);
	void Remove(shared_ptr<GameObject> gameObject);

	void Clear();
	
	void Update();
	void FixedUpdate();
	void WriteWorldStateToStream();

private:
	WorldServer();
	~WorldServer() {}

	system_clock::time_point _lastFixedUpdateTime;
	system_clock::time_point _lastPacketUpdateTime;

	vector<shared_ptr<GameObject>> _gameObjects;

	queue<shared_ptr<GameObject>> _pendingSerializationQueue;

	LinkingContext& _linkingContext;
};
