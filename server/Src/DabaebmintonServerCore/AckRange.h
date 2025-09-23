#pragma once

#include "InFlightPacket.h"

class InputMemoryBitStream;

class AckRange
{
public:
	PacketSequenceNumber GetStart() const { return _start; }
	uint32_t GetCount() const { return _count; }

	void Read(InputMemoryBitStream& inStream);

private:
	PacketSequenceNumber _start = 0;
	uint32_t _count = 0; // circular 구현 위해 end 아닌 count 사용
};
