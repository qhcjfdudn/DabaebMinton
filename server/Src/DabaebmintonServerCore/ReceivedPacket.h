#pragma once

#include "SockAddress.h"
#include "InputMemoryBitStream.h"

class ReceivedPacket
{
public:
	ReceivedPacket(shared_ptr<InputMemoryBitStream> inStreamPtr, const SockAddress& sockAddress);

	const SockAddress& GetSockAddress() const { return _sockAddress; }
	shared_ptr<InputMemoryBitStream> GetStream() { return _inStreamPtr; }

private:
	SockAddress _sockAddress;
	shared_ptr<InputMemoryBitStream> _inStreamPtr;
};
