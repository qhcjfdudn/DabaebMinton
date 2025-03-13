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

private:
	char* _buffer;
	unsigned int _len;
};

class PacketQueue
{
public:
	static PacketQueue& GetReceiveStaticInstance();
	static PacketQueue& GetSendStaticInstance();

	void PushCopy(Packet packet);
	shared_ptr<Packet> Front();
	bool Empty();

private:
	PacketQueue() {}
	~PacketQueue() {}

	// NetworkManager와 World의 서로 다른 thread에서 접근할테니
	// lock/unlock 고민 필요
	std::queue<shared_ptr<Packet>> _bufferQueue;
};
