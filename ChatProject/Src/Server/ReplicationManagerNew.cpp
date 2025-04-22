#include "ServerPCH.h"
#include "ReplicationManagerNew.h"

#include "ReplicationHeader.h"
#include "LinkingContextNew.h"
#include "GameObjectNew.h"

void ReplicationManagerNew::ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObjectNew> inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

void ReplicationManagerNew::ReplicateDelete(OutputMemoryBitStream& inStream, const shared_ptr<GameObjectNew> inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Delete,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());
	rh.Write(inStream);
	inGameObject->Write(inStream);
}

ReplicationManagerNew::ReplicationManagerNew() :
	linkingContext{} {
}
