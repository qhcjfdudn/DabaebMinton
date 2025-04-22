#include "ServerPCH.h"
#include "LinkingContextNew.h"

void LinkingContextNew::Clear()
{
	_networkIdToGameObjectMap.clear();
	_gameObjectToNetworkIdMap.clear();
	_nextNetworkId = 1;
}

NetworkId_t LinkingContextNew::GetNetworkId(shared_ptr<GameObjectNew> gameObject)
{
	if (_gameObjectToNetworkIdMap.find(gameObject) == _gameObjectToNetworkIdMap.end())
	{
		return 0;
	}

	return _gameObjectToNetworkIdMap[gameObject];
}
shared_ptr<GameObjectNew> LinkingContextNew::GetGameObject(NetworkId_t networkId)
{
	return _networkIdToGameObjectMap[networkId];
}
void LinkingContextNew::AddGameObject(shared_ptr<GameObjectNew> gameObject)
{
	_networkIdToGameObjectMap[_nextNetworkId] = gameObject;
	_gameObjectToNetworkIdMap[gameObject] = _nextNetworkId;
	++_nextNetworkId;
}
void LinkingContextNew::RemoveGameObject(NetworkId_t networkId)
{
	auto gameObject = _networkIdToGameObjectMap[networkId];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}

void LinkingContextNew::RemoveGameObject(shared_ptr<GameObjectNew> gameObject)
{
	NetworkId_t networkId = _gameObjectToNetworkIdMap[gameObject];
	_networkIdToGameObjectMap.erase(networkId);
	_gameObjectToNetworkIdMap.erase(gameObject);
}
