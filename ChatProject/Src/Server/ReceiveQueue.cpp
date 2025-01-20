#include "ServerPCH.h"
#include "ReceiveQueue.h"

ReceiveQueue& ReceiveQueue::GetInstance()
{
	static ReceiveQueue sInstance;
	return sInstance;
}

void ReceiveQueue::Push(const string& str)
{
	_bufferQueue.push(str);
}

string ReceiveQueue::Front()
{
	string ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool ReceiveQueue::Empty()
{
	return _bufferQueue.empty();
}
