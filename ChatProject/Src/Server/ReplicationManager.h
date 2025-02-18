#pragma once

#include "LinkingContext.h"

class ReplicationManager
{
public:
	static ReplicationManager& GetInstance();

private:
	ReplicationManager() :
		_linkingContext(LinkingContext::GetInstance()) {}
	~ReplicationManager() = default;

	LinkingContext& _linkingContext;
};

