#pragma once

class LinkingContext;
class OutputMemoryBitStream;
class GameObject;

class ReplicationManager
{
public:
	static ReplicationManager GetInstance();

	void ReplicateCreate(OutputMemoryBitStream & outStream, LinkingContext& linkingContext, const GameObject * gameObject);
	void ReplicateUpdate(OutputMemoryBitStream & outStream, const LinkingContext& linkingContext, const GameObject * gameObject);
	void ReplicateUpdate(OutputMemoryBitStream & outStream, const LinkingContext& linkingContext, const vector<shared_ptr<GameObject>>& gameObjects);
	void ReplicateDelete(OutputMemoryBitStream & outStream, LinkingContext& linkingContext, const GameObject * gameObject);

private:
	ReplicationManager() = default;
};
