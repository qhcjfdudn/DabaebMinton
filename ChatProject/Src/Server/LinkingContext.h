#pragma once

class GameObject;

class LinkingContext
{
public:
	static LinkingContext& GetInstance();

	unsigned int GetNetworkId(shared_ptr<GameObject> gameObject);
	shared_ptr<GameObject> GetGameObject(unsigned int networkId);
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(unsigned int networkId);

private:
	LinkingContext() = default;
	~LinkingContext() = default;

	std::unordered_map<unsigned int, shared_ptr<GameObject>> _networkIdToGameObjectMap;
	std::unordered_map<shared_ptr<GameObject>, unsigned int> _gameObjectToNetworkIdMap;
	
	unsigned int _nextNetworkId = 1;
};

