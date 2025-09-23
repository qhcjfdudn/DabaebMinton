#include "ServerPCH.h"
#include "ReceivedPacket.h"

ReceivedPacket::ReceivedPacket(shared_ptr<InputMemoryBitStream> inStreamPtr, const SockAddress& sockAddress) :
	_sockAddress(sockAddress), _inStreamPtr(inStreamPtr)
{
}
