#include "ServerPCH.h"
#include "LinkingContext.h"

LinkingContext& LinkingContext::GetInstance()
{
	static LinkingContext instance;
	return instance;
}

unsigned int LinkingContext::GetNetworkId(shared_ptr<GameObject> gameObject)
{
	if (_gameObjectToNetworkIdMap.find(gameObject) == _gameObjectToNetworkIdMap.end())
	{
		_gameObjectToNetworkIdMap[gameObject] = _nextNetworkId;
		_networkIdToGameObjectMap[_nextNetworkId] = gameObject;
		++_nextNetworkId;
	}

	return _gameObjectToNetworkIdMap[gameObject];
}

shared_ptr<GameObject> LinkingContext::GetGameObject(unsigned int networkId)
{
	return _networkIdToGameObjectMap[networkId];
}

void LinkingContext::AddGameObject(shared_ptr<GameObject> gameObject, unsigned int networkId)
{
	_networkIdToGameObjectMap[networkId] = gameObject;
	_gameObjectToNetworkIdMap[gameObject] = networkId;
}

void LinkingContext::RemoveGameObject(unsigned int networkId)
{
	auto gameObject = _networkIdToGameObjectMap[networkId];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}
