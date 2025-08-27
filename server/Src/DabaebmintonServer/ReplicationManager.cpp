#include "ServerPCH.h"
#include "ReplicationManager.h"

#include "ReplicationHeader.h"
#include "LinkingContext.h"
#include "GameObject.h"

#include "Constant.h"

ReplicationManager::ReplicationManager() :
	linkingContext{},
	_replicationInterval{ Constant::PACKET_PERIOD }, 
	_timeSinceLastReplication{ system_clock::now() }
{
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, GameObject* inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Update,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());

	rh.Write(inStream);
	inGameObject->Write(inStream);
}

void ReplicationManager::ReplicateUpdate(OutputMemoryBitStream& inStream, vector<GameObject*> inGameObjects)
{
	for (const auto& go : inGameObjects)
		ReplicateUpdate(inStream, go);
}

void ReplicationManager::ReplicateDelete(OutputMemoryBitStream& inStream, GameObject* const inGameObject)
{
	ReplicationHeader rh(ReplicationHeader::ReplicationAction::RA_Delete,
		linkingContext.GetNetworkId(inGameObject),
		inGameObject->GetClassId());
	rh.Write(inStream);
	inGameObject->Write(inStream);
}

bool ReplicationManager::HasElapsedReplicationInterval() const
{
	system_clock::time_point currentTime = system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - _timeSinceLastReplication;

	return elapsedTime.count() >= _replicationInterval;
}
