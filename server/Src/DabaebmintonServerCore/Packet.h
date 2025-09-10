#pragma once

class Packet
{
public:
	Packet(const char* src, unsigned int bytes);
	Packet(const Packet& src);
	Packet(Packet&& src) noexcept;
	~Packet();

	const char* GetBuffer() const;
	unsigned int GetLength() const;

	void PrintInHex() const;
	void Print() const;

private:
	char* _buffer;
	unsigned int _len;
};

enum class PacketType
{
	PT_Hello,
	PT_ReplicationData,
	PT_RPC,
	PT_Disconnect,
	PT_Max
};