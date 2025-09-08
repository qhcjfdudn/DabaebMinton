#include "ServerPCH.h"
#include "ReplicationManager.h"

#include "ReplicationHeader.h"
#include "LinkingContext.h"
#include "GameObject.h"

#include "Constant.h"

ReplicationManager ReplicationManager::GetInstance()
{
	static ReplicationManager instance;
	return instance;
}

void ReplicationManager::ReplicateCreate(OutputMemoryBitStream& inStream, LinkingContext& linkingContext, const GameObject* inGameObject)
{
	linkingContext.AddGameObject(inGameObject);

	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Create,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, const LinkingContext& linkingContext, const GameObject* inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

void ReplicationManager::ReplicateDelete(OutputMemoryBitStream& inStream, LinkingContext& linkingContext, const GameObject* inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Delete,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());
	rh.Write(inStream);
	inGameObject->Write(inStream);

	linkingContext.RemoveGameObject(inGameObject);
}