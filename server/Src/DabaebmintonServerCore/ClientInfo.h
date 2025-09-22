#pragma once

#include "OverlappedDto.h"
#include "ReplicationManager.h"
#include "DeliveryNotificationManager.h"
#include "SockAddress.h"

class ClientInfo
{
public:
	ClientInfo(std::string_view ip, const uint16_t port);
	ClientInfo(const SockAddress& sockAddress);

	const SockAddress& GetSockAddress() const { return _sockAddress; }

	ReplicationManager& GetReplicationManager() { return _replicationManager; }
	DeliveryNotificationManager& GetDeliveryNotificationManager() { return _deliveryNotificationManager; }

private:
	SockAddress _sockAddress;

	// for RUDP
	ReplicationManager _replicationManager{};
	DeliveryNotificationManager _deliveryNotificationManager{};
};
