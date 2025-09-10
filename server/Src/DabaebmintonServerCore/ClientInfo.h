#pragma once

#include "Constant.h"
#include "OutputMemoryBitStream.h"
#include "OverlappedDto.h"

class ClientInfo
{
public:
	ClientInfo(const string& ip, const unsigned int port);

	const std::string _ipPort;

	// for RUDP + IOCP
	uint8_t m_sequenceNoInChannels[Constant::RUDP_MAX_CHANNEL_SIZE] = {};
	OverlappedDto m_OverlappedDtdo[Constant::RUDP_MAX_CHANNEL_SIZE] = {};
	
	OutputMemoryBitStream m_pendingStreamToSendingInChannels[Constant::RUDP_MAX_CHANNEL_SIZE] = {};
};
