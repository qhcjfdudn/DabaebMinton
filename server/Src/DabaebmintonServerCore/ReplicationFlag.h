#pragma once
enum class ReplicationFlag {
	DF_NONE = 0,
	DF_CREATE = 1,
	DF_UPDATE = 2,
	DF_DELETE = 4,
	DF_ALL = DF_CREATE | DF_UPDATE | DF_DELETE
};
ReplicationFlag operator| (ReplicationFlag lhs, ReplicationFlag rhs);
ReplicationFlag& operator|= (ReplicationFlag& lhs, ReplicationFlag rhs);
ReplicationFlag operator& (ReplicationFlag lhs, ReplicationFlag rhs);

bool hasFlag(ReplicationFlag flags, ReplicationFlag flag);