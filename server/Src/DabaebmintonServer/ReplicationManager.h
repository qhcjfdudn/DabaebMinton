#pragma once

#include "LinkingContext.h"

class OutputMemoryBitStream;
class GameObject;

class ReplicationManager
{
public:
	ReplicationManager();

	void ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObject> inGameObject);
	void ReplicateUpdate(OutputMemoryBitStream& inStream, vector<shared_ptr<GameObject> > inGameObjects);

	void ReplicateDelete(OutputMemoryBitStream& inStream, const shared_ptr<GameObject> inGameObject);

	bool HasElapsedReplicationInterval() const;

	LinkingContext linkingContext;

private:
	double _replicationInterval;	
	system_clock::time_point _timeSinceLastReplication;
};
