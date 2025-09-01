#pragma once

#include "OutputMemoryBitStream.h"
#include "OverlappedDto.h"

class ClientInfo
{
public:
	ClientInfo(ULONG_PTR completionKey, const sockaddr_in& addr);

	ULONG_PTR _completionKey;
	sockaddr_in _addr;

	// for RUDP + IOCP
	const int _channelCount = 2;
	int m_sequenceNoInChannels[2];
	OverlappedDto m_receiveOverlappedDto, m_sendOverlappedDtdo[2];
	
	OutputMemoryBitStream _pendingStreamToSendingInChannels[2];
};

