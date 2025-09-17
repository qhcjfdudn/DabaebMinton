#pragma once

#include "Constant.h"
#include "OutputMemoryBitStream.h"
#include "OverlappedDto.h"
#include "ReplicationManager.h"
#include "DeliveryNotificationManager.h"

class ClientInfo
{
public:
	ClientInfo(const string& ip, const unsigned int port);

	ReplicationManager& GetReplicationManager() { return _replicationManager; }
	DeliveryNotificationManager& GetDeliveryNotificationManager() { return _deliveryNotificationManager; }

	const std::string _ipPort;

	// for RUDP + IOCP
	uint8_t m_sequenceNoInChannels[Constant::RUDP_MAX_CHANNEL_SIZE] = {};
	OverlappedDto m_OverlappedDtdo[Constant::RUDP_MAX_CHANNEL_SIZE] = {};
	
	OutputMemoryBitStream m_pendingStreamToSendingInChannels[Constant::RUDP_MAX_CHANNEL_SIZE] = {};

private:
	ReplicationManager _replicationManager{};
	DeliveryNotificationManager _deliveryNotificationManager{};
};
