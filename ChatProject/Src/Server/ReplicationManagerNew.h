#pragma once

#include "LinkingContextNew.h"

class OutputMemoryBitStream;
class GameObjectNew;

class ReplicationManagerNew
{
public:
	ReplicationManagerNew();

	void ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObjectNew> inGameObject);
	void ReplicateDelete(OutputMemoryBitStream& inStream, const shared_ptr<GameObjectNew> inGameObject);

	LinkingContextNew linkingContext;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_Disconnect,
	PT_Max
};