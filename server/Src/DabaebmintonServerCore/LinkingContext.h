#pragma once

class GameObject;

class LinkingContext
{
public:
	void Clear();

	NetworkId_t GetNetworkId(GameObject* gameObject);
	GameObject* GetGameObject(NetworkId_t networkId);
	void AddGameObject(GameObject* gameObject);
	void RemoveGameObject(NetworkId_t networkId);
	void RemoveGameObject(GameObject* gameObject);

private:
	std::unordered_map<NetworkId_t, GameObject*> _networkIdToGameObjectMap;
	std::unordered_map<GameObject*, NetworkId_t> _gameObjectToNetworkIdMap;

	NetworkId_t _nextNetworkId = 1;
};

