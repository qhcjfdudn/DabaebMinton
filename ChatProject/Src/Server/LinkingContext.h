#pragma once

class GameObject;

class LinkingContext
{
public:
	void Clear();

	NetworkId_t GetNetworkId(shared_ptr<GameObject> gameObject);
	shared_ptr<GameObject> GetGameObject(NetworkId_t networkId);
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(NetworkId_t networkId);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

private:
	std::unordered_map<NetworkId_t, shared_ptr<GameObject>> _networkIdToGameObjectMap;
	std::unordered_map<shared_ptr<GameObject>, NetworkId_t> _gameObjectToNetworkIdMap;

	NetworkId_t _nextNetworkId = 1;
};

