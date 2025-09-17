#pragma once

class GameObject;

class LinkingContext
{
public:
	void Clear();

	GameObject* GetGameObject(NetworkId_t networkId) const;
	NetworkId_t RegisterGameObject(shared_ptr<GameObject> gameObject);
	void UnregisterGameObject(NetworkId_t networkId);

private:
	std::unordered_map<NetworkId_t, shared_ptr<GameObject>> _networkIdToGameObjectMap;

	NetworkId_t _nextNetworkId{ 1 };
};
