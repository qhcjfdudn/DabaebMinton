#include "ServerPCH.h"
#include "LinkingContext.h"

LinkingContext& LinkingContext::GetInstance()
{
	static LinkingContext instance;
	return instance;
}

NetworkId_t LinkingContext::GetNetworkId(shared_ptr<GameObject> gameObject)
{
	if (_gameObjectToNetworkIdMap.find(gameObject) == _gameObjectToNetworkIdMap.end())
	{
		return 0;
	}

	return _gameObjectToNetworkIdMap[gameObject];
}
shared_ptr<GameObject> LinkingContext::GetGameObject(NetworkId_t networkId)
{
	return _networkIdToGameObjectMap[networkId];
}
void LinkingContext::AddGameObject(shared_ptr<GameObject> gameObject)
{
	_networkIdToGameObjectMap[_nextNetworkId] = gameObject;
	_gameObjectToNetworkIdMap[gameObject] = _nextNetworkId;
	++_nextNetworkId;
}
void LinkingContext::RemoveGameObject(NetworkId_t networkId)
{
	auto gameObject = _networkIdToGameObjectMap[networkId];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}

void LinkingContext::RemoveGameObject(shared_ptr<GameObject> gameObject)
{
	NetworkId_t networkId = _gameObjectToNetworkIdMap[gameObject];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}
