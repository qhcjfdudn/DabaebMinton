#include "ServerPCH.h"
#include "SendQueue.h"

SendQueue& SendQueue::GetInstance()
{
	static SendQueue sInstance;
	return sInstance;
}

void SendQueue::Push(const string& str)
{
	cout << "str: " << str << endl;
	_bufferQueue.push(str);
}

string SendQueue::Front()
{
	string ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool SendQueue::Empty()
{
	return _bufferQueue.empty();
}
