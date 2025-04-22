#pragma once

class GameObjectNew;

class LinkingContextNew
{
public:
	void Clear();

	NetworkId_t GetNetworkId(shared_ptr<GameObjectNew> gameObject);
	shared_ptr<GameObjectNew> GetGameObject(NetworkId_t networkId);
	void AddGameObject(shared_ptr<GameObjectNew> gameObject);
	void RemoveGameObject(NetworkId_t networkId);
	void RemoveGameObject(shared_ptr<GameObjectNew> gameObject);

private:
	std::unordered_map<NetworkId_t, shared_ptr<GameObjectNew>> _networkIdToGameObjectMap;
	std::unordered_map<shared_ptr<GameObjectNew>, NetworkId_t> _gameObjectToNetworkIdMap;

	NetworkId_t _nextNetworkId = 1;
};

