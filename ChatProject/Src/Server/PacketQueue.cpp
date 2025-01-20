#include "ServerPCH.h"
#include "PacketQueue.h"

PacketQueue& PacketQueue::GetInstance()
{
	static PacketQueue sInstance;
	return sInstance;
}

void PacketQueue::Push(const string& str)
{
	_bufferQueue.push(str);
}

string PacketQueue::Front()
{
	string& ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool PacketQueue::Empty()
{
	return _bufferQueue.empty();
}
