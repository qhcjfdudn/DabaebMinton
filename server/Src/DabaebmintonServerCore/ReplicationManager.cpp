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

void ReplicationManager::ReplicateCreate(OutputMemoryBitStream & outStream, LinkingContext& linkingContext, const GameObject * gameObject)
{
	linkingContext.AddGameObject(gameObject);

	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Create,
		linkingContext.GetNetworkId(gameObject),
		gameObject->GetClassId());

	rh.Write(outStream);
	gameObject->Write(outStream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& outStream, const LinkingContext& linkingContext, const GameObject* gameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		linkingContext.GetNetworkId(gameObject),
		gameObject->GetClassId());

	rh.Write(outStream);
	gameObject->Write(outStream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& outStream, const LinkingContext& linkingContext, const vector<shared_ptr<GameObject>>& gameObjects)
{
	for (auto& gameObject : gameObjects)
		ReplicateUpdate(outStream, linkingContext, gameObject.get());
}

void ReplicationManager::ReplicateDelete(OutputMemoryBitStream& outStream, LinkingContext& linkingContext, const GameObject* gameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Delete,
		linkingContext.GetNetworkId(gameObject),
		gameObject->GetClassId());
	rh.Write(outStream);
	gameObject->Write(outStream);

	linkingContext.RemoveGameObject(gameObject);
}