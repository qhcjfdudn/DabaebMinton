#pragma once

#include "OutputMemoryBitStream.h"
#include "OverlappedDto.h"

class ClientInfo
{
public:
	ClientInfo(const string& ip, const unsigned int port);

	std::string _ipPort;

	// for RUDP + IOCP
	const int _channelCount = 2;
	int m_sequenceNoInChannels[2] = {};
	OverlappedDto m_receiveOverlappedDto{}, m_sendOverlappedDtdo[2] = {};
	
	OutputMemoryBitStream _pendingStreamToSendingInChannels[2] = {};
};
