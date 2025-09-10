#pragma once

#include "Packet.h"

class OutputMemoryBitStream;
class GameObject;

class RUDPPacketizer
{
public:
	static RUDPPacketizer& GetInstance();

	vector<shared_ptr<Packet>> Packetize(
		const uint8_t channelId,
		uint8_t& outSeqNum,
		const PacketType packetType,
		vector<shared_ptr<GameObject>> gameObjects);

	vector<shared_ptr<Packet>> PacketizeReliable(
		const int channelId,
		uint8_t& outSeqNum,
		const PacketType packetType,
		OutputMemoryBitStream& inStream); // InputMemoryBitStream으로 변경 필요. 현재 미구현.

private:
	RUDPPacketizer() = default;
};
