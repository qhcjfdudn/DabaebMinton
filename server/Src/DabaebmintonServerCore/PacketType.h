#pragma once

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_RPC,
	PT_Disconnect,
	PT_Max
};
