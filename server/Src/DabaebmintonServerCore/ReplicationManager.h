#pragma once

#include "ReplicationCommand.h"

class OutputMemoryBitStream;
class PacketGenerator;

class ReplicationManager
{
public:
	void ReplicateCreate(int inNetworkId, uint8_t inInitialDirtyState);
	void ReplicateDestroy(int inNetworkId);
	void SetStateDirty(int inNetworkId, uint8_t inDirtyState);
	void HandleCreateAckd(int inNetworkId);
	void RemoveFromReplication(int inNetworkId);

	void Write(PacketGenerator& packetGenerator);

	bool ShouldReplicate = false;

private:
	size_t CountWriteBitSize(NetworkId_t networkId) const;

	uint8_t WriteCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState);
	uint8_t WriteUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState);
	uint8_t WriteDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint8_t inDirtyState);

	unordered_map<NetworkId_t, ReplicationCommand>	mNetworkIdToReplicationCommand;
	shared_mutex _networkIdToReplicationCommandMutex{};
};
