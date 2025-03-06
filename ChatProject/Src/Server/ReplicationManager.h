#pragma once

#include "OutputMemoryBitStream.h"
#include "LinkingContext.h"

class ReplicationManager
{
public:
	static ReplicationManager& GetInstance();

	void ReplicateUpdate(OutputMemoryBitStream& inStream,
		GameObject* inGameObject);

private:
	ReplicationManager() :
		_linkingContext(LinkingContext::GetInstance()) {}
	~ReplicationManager() = default;

	LinkingContext& _linkingContext;
};

