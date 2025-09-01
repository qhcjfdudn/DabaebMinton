#pragma once

#include "LinkingContext.h"

class OutputMemoryBitStream;
class GameObject;

class ReplicationManager
{
public:
	ReplicationManager();

	void ReplicateUpdate(OutputMemoryBitStream& inStream, GameObject* inGameObject);
	void ReplicateUpdate(OutputMemoryBitStream& inStream, vector<GameObject*> inGameObjects);

	void ReplicateDelete(OutputMemoryBitStream& inStream, GameObject* const inGameObject);

	bool HasElapsedReplicationInterval() const;

	LinkingContext linkingContext;

private:
	double _replicationInterval;	
	system_clock::time_point _timeSinceLastReplication;
};
