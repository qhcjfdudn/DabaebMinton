#include "ServerPCH.h"
#include "Packet.h"

#include <spdlog/spdlog.h>

Packet::Packet(const char* src, unsigned int bytes) :
	_len(bytes)
{
	_buffer = new unsigned char[bytes];
	memcpy(_buffer, src, bytes);
}

Packet::Packet(const Packet& src) :
	_len(src._len)
{
	_buffer = new unsigned char[_len];
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

const unsigned char* Packet::GetBuffer() const
{
	return _buffer;
}

size_t Packet::GetLength() const
{
	return _len;
}

string Packet::GetInHex() const
{
	if (_len == 0)
		return "";

	string ret = "";
	ret.reserve((_len << 1) + (_len - 1) / 4);

	for (int i = 0, size = static_cast<int>(_len); i < size; ++i)
	{
		if (i > 0 && (i & 3) == 0)
			ret += ' ';

		ret += fmt::format("{:02X}", _buffer[i]);
	}

	return ret;
}
