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

const char* Packet::GetBuffer() const
{
	return _buffer;
}

unsigned int Packet::GetLength() const
{
	return _len;
}

void Packet::PrintInHex() const
{
	cout.setf(std::ios::hex, std::ios::basefield);
	cout.setf(std::ios::uppercase);
	for (unsigned int i = 0; i < _len; ++i)
	{
		cout << std::setw(2) << std::setfill('0') << static_cast<int>((static_cast<unsigned char>(_buffer[i])));
		cout << " ";
	}
	cout << endl;
	cout.unsetf(std::ios::hex);
	cout.unsetf(std::ios::uppercase);
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
