#include "ServerPCH.h"
#include "LinkingContext.h"

void LinkingContext::Clear()
{
	_networkIdToGameObjectMap.clear();
	_gameObjectToNetworkIdMap.clear();
	_nextNetworkId = 1;
}

NetworkId_t LinkingContext::GetNetworkId(const GameObject* gameObject) const
{
	if (_gameObjectToNetworkIdMap.find(gameObject) == _gameObjectToNetworkIdMap.end())
	{
		return 0;
	}

	return _gameObjectToNetworkIdMap.at(gameObject);
}
const GameObject* LinkingContext::GetGameObject(NetworkId_t networkId) const
{
	if (_networkIdToGameObjectMap.find(networkId) == _networkIdToGameObjectMap.end())
		return nullptr;

	return _networkIdToGameObjectMap.at(networkId);
}
void LinkingContext::AddGameObject(const GameObject* gameObject)
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

void LinkingContext::RemoveGameObject(const GameObject* gameObject)
{
	NetworkId_t networkId = _gameObjectToNetworkIdMap[gameObject];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}
