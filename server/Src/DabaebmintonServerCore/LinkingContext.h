#pragma once

class GameObject;

class LinkingContext
{
public:
	void Clear();

	NetworkId_t GetNetworkId(const GameObject* gameObject) const;
	const GameObject* GetGameObject(NetworkId_t networkId) const;
	void AddGameObject(const GameObject* gameObject);
	void RemoveGameObject(NetworkId_t networkId);
	void RemoveGameObject(const GameObject* gameObject);

private:
	std::unordered_map<NetworkId_t, const GameObject*> _networkIdToGameObjectMap;
	std::unordered_map<const GameObject*, NetworkId_t> _gameObjectToNetworkIdMap;

	NetworkId_t _nextNetworkId = 1;
};
