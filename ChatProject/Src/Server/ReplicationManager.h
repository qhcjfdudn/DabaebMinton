#pragma once

class OutputMemoryBitStream;
class GameObject;
class LinkingContext;

class ReplicationManager
{
public:
	static ReplicationManager& GetInstance();

	void ReplicateUpdate(OutputMemoryBitStream& inStream, shared_ptr<GameObject> inGameObject);

	LinkingContext& _linkingContext;

private:
	ReplicationManager();
	~ReplicationManager() = default;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_Disconnect,
	PT_Max
};