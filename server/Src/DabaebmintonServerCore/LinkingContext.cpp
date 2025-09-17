#include "ServerPCH.h"
#include "LinkingContext.h"

#include "GameObject.h"

void LinkingContext::Clear()
{
	_networkIdToGameObjectMap.clear();
	_nextNetworkId = 1;
}

GameObject* LinkingContext::GetGameObject(NetworkId_t networkId) const
{
	if (_networkIdToGameObjectMap.find(networkId) == _networkIdToGameObjectMap.end())
		return nullptr;

	return _networkIdToGameObjectMap.at(networkId).get();
}
NetworkId_t LinkingContext::RegisterGameObject(shared_ptr<GameObject> gameObject)
{
	NetworkId_t networkId = _nextNetworkId++;

	gameObject->SetNetworkId(networkId);
	_networkIdToGameObjectMap.emplace(networkId, gameObject);

	return networkId;
}
void LinkingContext::UnregisterGameObject(NetworkId_t networkId)
{
	if (_networkIdToGameObjectMap.find(networkId) == _networkIdToGameObjectMap.end())
		return;

	_networkIdToGameObjectMap.erase(networkId);
}
