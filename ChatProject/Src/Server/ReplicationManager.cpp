#include "ServerPCH.h"
#include "ReplicationManager.h"

#include "ReplicationHeader.h"
#include "GameObject.h"
#include "LinkingContext.h"

ReplicationManager& ReplicationManager::GetInstance()
{
	static ReplicationManager instance;
	return instance;
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObject> inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		_linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

ReplicationManager::ReplicationManager() :
	_linkingContext(LinkingContext::GetInstance()) {
}
