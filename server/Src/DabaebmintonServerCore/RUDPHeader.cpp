#include "ServerPCH.h"
#include "RUDPHeader.h"

#include "OutputMemoryBitStream.h"

RUDPHeader::RUDPHeader(
	uint8_t channelId,
	uint8_t sequenceNumber,
	uint32_t totalBitLength,
	uint32_t offset, 
	uint16_t chunkLength) :
	_channelId(channelId),
	_sequenceNumber(sequenceNumber),
	_totalBitLength(totalBitLength),
	_offset(offset),
	_chunkLength(chunkLength)
{
}

void RUDPHeader::Write(OutputMemoryBitStream& outStream) const
{
	outStream.WriteBits(&_channelId, GetRequiredBits(Constant::RUDP_MAX_CHANNEL_SIZE));
	outStream.Write(_sequenceNumber);
	outStream.Write(_totalBitLength);
	outStream.Write(_offset);
	outStream.Write(_chunkLength);
}
