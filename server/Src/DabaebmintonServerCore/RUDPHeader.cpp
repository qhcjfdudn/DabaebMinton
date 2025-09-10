#include "ServerPCH.h"
#include "RUDPHeader.h"

#include "OutputMemoryBitStream.h"
#include "Constant.h"
#include "GetRequiredBits.h"

RUDPHeader::RUDPHeader(uint8_t channelId, uint8_t sequenceNumber, uint32_t totalLength, uint32_t offset, uint32_t chunkLength) :
	_channelId(channelId), 
	_sequenceNumber(sequenceNumber),
	_totalLength(totalLength),
	_offset(offset),
	_chunkLength(chunkLength)
{}

void RUDPHeader::Write(OutputMemoryBitStream& outStream) const
{
	outStream.WriteBits(&_channelId, GetRequiredBits(Constant::RUDP_MAX_CHANNEL_SIZE));
	outStream.Write(_sequenceNumber);
	outStream.Write(_totalLength);
	outStream.Write(_offset);
	outStream.Write(_chunkLength);
}
