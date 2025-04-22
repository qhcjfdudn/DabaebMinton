#pragma once

#include "LinkingContext.h"

class OutputMemoryBitStream;
class GameObject;

class ReplicationManager
{
public:
	ReplicationManager();

	void ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObject> inGameObject);
	void ReplicateDelete(OutputMemoryBitStream& inStream, const shared_ptr<GameObject> inGameObject);

	LinkingContext linkingContext;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_Disconnect,
	PT_Max
};