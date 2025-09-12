#pragma once

#include "Constant.h"
#include "GetRequiredBits.h"

class OutputMemoryBitStream;

class RUDPHeader
{
public:
	RUDPHeader(
		uint8_t channelId,
		uint8_t sequenceNumber,
		uint32_t totalBitLength = 0,
		uint32_t offset = 0,
		uint16_t chunkLength = 0);

	void Write(OutputMemoryBitStream& outStream) const;

	static constexpr size_t CountsHeaderBits()
	{
		return GetRequiredBits(Constant::RUDP_MAX_CHANNEL_SIZE)
			+ (sizeof(_sequenceNumber) << 3)
			+ (sizeof(_totalBitLength) << 3)
			+ (sizeof(_offset) << 3)
			+ (sizeof(_chunkLength) << 3);
	}

private:
	uint8_t _channelId;
	uint8_t _sequenceNumber;
	uint32_t _totalBitLength;   // 원본 메시지 전체 길이 (모든 fragment에 동일)
	uint32_t _offset;        // 이 fragment의 시작 위치
	uint16_t _chunkLength;   // size < MTU
};
