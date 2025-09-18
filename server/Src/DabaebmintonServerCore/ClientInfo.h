#pragma once

#include "OverlappedDto.h"
#include "ReplicationManager.h"
#include "DeliveryNotificationManager.h"

class ClientInfo
{
public:
	ClientInfo(const string& ip, const unsigned int port);

	sockaddr_in* GetSockAddr() { return &_sockAddr; }

	ReplicationManager& GetReplicationManager() { return _replicationManager; }
	DeliveryNotificationManager& GetDeliveryNotificationManager() { return _deliveryNotificationManager; }

	const std::string _ipPort;

	// for IOCP
	OverlappedDto _sendOverlappedDto;

private:
	sockaddr_in _sockAddr;

	// for RUDP
	ReplicationManager _replicationManager{};
	DeliveryNotificationManager _deliveryNotificationManager{};
};
