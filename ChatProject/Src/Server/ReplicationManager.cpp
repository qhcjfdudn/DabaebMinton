#include "ServerPCH.h"
#include "ReplicationManager.h"

ReplicationManager& ReplicationManager::GetInstance()
{
	static ReplicationManager instance;
	return instance;
}
void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, GameObject* inGameObject)
{
	/*
		ReplicationHeader rh(ReplicationAction::RA_Update,
			mLinkingContext->GetNetworkId(inGameObject, false),
			inGameObject->GetClassId());

		rh.Write(inStream);
		inGameObject->Write(inStream);
	*/
}
