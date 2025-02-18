#include "ServerPCH.h"
#include "ReplicationManager.h"

ReplicationManager& ReplicationManager::GetInstance()
{
	static ReplicationManager instance;
	return instance;
}
