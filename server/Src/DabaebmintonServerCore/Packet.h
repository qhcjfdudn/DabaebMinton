#pragma once

class Packet
{
public:
	Packet(const unsigned char * src, unsigned int bytes);
	Packet(const Packet& src);
	Packet(Packet&& src) noexcept;
	~Packet();

	const unsigned char* GetBuffer() const;
	size_t GetLength() const;

	string GetInHex() const;

private:
	unsigned char* _buffer;
	size_t _len;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_RPC,
	PT_Disconnect,
	PT_Max
};