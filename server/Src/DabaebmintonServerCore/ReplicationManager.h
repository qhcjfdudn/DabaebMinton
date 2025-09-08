#pragma once

class LinkingContext;
class OutputMemoryBitStream;
class GameObject;

class ReplicationManager
{
public:
	static ReplicationManager GetInstance();

	void ReplicateCreate(OutputMemoryBitStream& inStream, LinkingContext& linkingContext, const GameObject* inGameObject);
	void ReplicateUpdate(OutputMemoryBitStream& inStream, const LinkingContext& linkingContext, const GameObject* inGameObject);
	void ReplicateDelete(OutputMemoryBitStream& inStream, LinkingContext& linkingContext, const GameObject* inGameObject);

private:
	ReplicationManager() = default;
};
