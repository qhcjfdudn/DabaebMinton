#pragma once

#include "ReplicationManager.h"
#include "DeliveryNotificationManager.h"
#include "SockAddress.h"

class ClientProxy
{
public:
	ClientProxy(std::string_view ip, const uint16_t port);
	ClientProxy(const SockAddress& sockAddress);

	const SockAddress& GetSockAddress() const { return _sockAddress; }

	ReplicationManager& GetReplicationManager() { return _replicationManager; }
	DeliveryNotificationManager& GetDeliveryNotificationManager() { return _deliveryNotificationManager; }

	void SetSessionTokenId(uint64_t sessionTokenId) { _sessionTokenId = sessionTokenId; }

private:
	SockAddress _sockAddress;

	// for RUDP
	ReplicationManager _replicationManager{};
	DeliveryNotificationManager _deliveryNotificationManager{};

	uint64_t _sessionTokenId;
};
