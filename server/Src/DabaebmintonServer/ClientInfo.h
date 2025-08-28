#pragma once

#include "OutputMemoryBitStream.h"

class ClientInfo
{
public:
	ULONG_PTR _completionKey;
	sockaddr_in addr;

	// for RUDP
	const int _channelCount = 2;

	OVERLAPPED _receivingOverlapped;
	OVERLAPPED _sendingOverlappedInChannels[2];
	int _sequenceNoInChannels[2];
	OutputMemoryBitStream _pendingStreamToSendingInChannels[2];
};

