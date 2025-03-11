#include "ServerPCH.h"
#include "PacketQueue.h"

Packet::Packet(const char* src, unsigned int bytes) :
	_len(bytes)
{
	_buffer = new char[bytes];
	memcpy(_buffer, src, bytes);
}

Packet::Packet(const Packet& src) :
	_len(src._len)
{
	_buffer = new char[_len];
	memcpy(_buffer, src._buffer, _len);
}

Packet::Packet(Packet&& src) noexcept :
	_len(src._len)
{
	_buffer = src._buffer;
	src._buffer = nullptr;
}
Packet::~Packet()
{
	if (_buffer == nullptr)
		return;

	free(_buffer);
}

const char* Packet::GetBuffer()
{
	return _buffer;
}

unsigned int Packet::GetLength()
{
	return _len;
}

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

void PacketQueue::PushCopy(Packet packet)
{
	_bufferQueue.push(make_shared<Packet>(packet));
}

shared_ptr<Packet> PacketQueue::Front()
{
	auto ret = _bufferQueue.front();
	_bufferQueue.pop();

	return ret;
}

bool PacketQueue::Empty()
{
	return _bufferQueue.empty();
}
