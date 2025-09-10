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
		const OutputMemoryBitStream& inStream); // 이거 InputMemoryBitStream 되어야 될 듯.

private:
	RUDPPacketizer() = default;
};
