#include "ServerPCH.h"
#include "ReplicationFlag.h"

ReplicationFlag operator| (ReplicationFlag lhs, ReplicationFlag rhs) {
	return static_cast<ReplicationFlag>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

ReplicationFlag& operator|= (ReplicationFlag& lhs, ReplicationFlag rhs) {
	lhs = lhs | rhs;
	return lhs;
}

ReplicationFlag operator&(ReplicationFlag lhs, ReplicationFlag rhs)
{
	return static_cast<ReplicationFlag>(static_cast<int>(lhs) & static_cast<int>(rhs));
}

bool hasFlag(ReplicationFlag flags, ReplicationFlag flag)
{
	return static_cast<bool>(flags & flag);
}