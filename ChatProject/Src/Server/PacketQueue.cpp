#include "ServerPCH.h"
#include "PacketQueue.h"

PacketQueue& PacketQueue::GetReceiveStaticInstance()
{
	static PacketQueue s_ReceiveInstance;
	return s_ReceiveInstance;
}

PacketQueue& PacketQueue::GetSendStaticInstance()
{
	static PacketQueue s_SendInstance;
	return s_SendInstance;
}

void PacketQueue::Push(const string& str)
{
	_bufferQueue.push(str);
}

string PacketQueue::Front()
{
	string ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool PacketQueue::Empty()
{
	return _bufferQueue.empty();
}
