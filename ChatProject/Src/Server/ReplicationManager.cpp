#include "ServerPCH.h"
#include "ReplicationManager.h"

#include "ReplicationHeader.h"
#include "LinkingContext.h"
#include "GameObject.h"

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObject> inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

void ReplicationManager::ReplicateDelete(OutputMemoryBitStream& inStream, const shared_ptr<GameObject> inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Delete,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());
	rh.Write(inStream);
	inGameObject->Write(inStream);
}

ReplicationManager::ReplicationManager() :
	linkingContext{} {
}
