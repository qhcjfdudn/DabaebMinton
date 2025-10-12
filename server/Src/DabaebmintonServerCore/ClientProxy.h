#pragma once

#include "SessionToken.h"
#include "SockAddress.h"
#include "ReplicationManager.h"
#include "DeliveryNotificationManager.h"

class ClientProxy
{
public:
	ClientProxy(const SessionToken& session);

	SessionToken GetSession() const { return _session; }

	const SockAddress& GetSockAddress() const { return _sockAddress; }
	void SetSockAddressIfAddressModified(const SockAddress& sockAddress);

	ReplicationManager& GetReplicationManager() { return _replicationManager; }
	DeliveryNotificationManager& GetDeliveryNotificationManager() { return _deliveryNotificationManager; }

private:
	SessionToken _session;
	SockAddress _sockAddress{};

	// for RUDP
	ReplicationManager _replicationManager{};
	DeliveryNotificationManager _deliveryNotificationManager{};
};
