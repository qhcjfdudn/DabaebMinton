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

void PacketQueue::PushCopy(const Packet& packet) {
	_bufferQueue.push(make_shared<Packet>(packet));
}

shared_ptr<Packet> PacketQueue::Front()
{
	auto ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool PacketQueue::Empty() const {
	return _bufferQueue.empty();
}
