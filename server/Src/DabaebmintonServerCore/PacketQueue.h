#pragma once

#include "Packet.h"

class PacketQueue
{
public:
	static PacketQueue& GetReceiveStaticInstance();
	static PacketQueue& GetSendStaticInstance();

	void PushCopy(const Packet& packet);
	shared_ptr<Packet> Front();
	bool Empty() const;

private:
	PacketQueue() {}
	~PacketQueue() {}

	// NetworkManager와 World의 서로 다른 thread에서 접근할테니
	// lock/unlock 고민 필요
	std::queue<shared_ptr<Packet>> _bufferQueue;
};
