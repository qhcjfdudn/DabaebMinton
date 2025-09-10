#include "ServerPCH.h"
#include "RUDPPacketizer.h"

#include "RUDPHeader.h"
#include "OutputMemoryBitStream.h"
#include "GameObject.h"
#include "Constant.h"
#include "GetRequiredBits.h"

RUDPPacketizer& RUDPPacketizer::GetInstance()
{
	static RUDPPacketizer instance;
	return instance;
}

vector<shared_ptr<Packet>> RUDPPacketizer::Packetize(
	const uint8_t channelId, 
	uint8_t& outSeqNum, 
	const PacketType packetType, 
	vector<shared_ptr<GameObject>> gameObjects)
{
	vector<shared_ptr<Packet>> ret;
	
	RUDPHeader dummyHeader{ 0, 0 };
	OutputMemoryBitStream dummyStream;
	dummyHeader.Write(dummyStream);

	const uint16_t BIT_HEADER_SIZE = dummyStream.GetBitLength();
	const uint16_t MAX_BIT_PAYLOAD_SIZE = (Constant::MAX_PACKET_SIZE << 3) - BIT_HEADER_SIZE;

	OutputMemoryBitStream packetStream, bufferStream;
	packetStream.Reserve(Constant::MAX_PACKET_SIZE);
	bufferStream.Reserve(Constant::MAX_PACKET_SIZE);

	int curIdx = 0;
	int goIdx = 0;
	int goSize = static_cast<int>(gameObjects.size());

	while (goIdx < goSize)
	{
		bufferStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
		uint16_t curLength = bufferStream.GetBitLength();

		for (; goIdx < goSize; ++goIdx)
		{
			gameObjects[goIdx]->Write(bufferStream);
			if (bufferStream.GetBitLength() > MAX_BIT_PAYLOAD_SIZE)
			{
				bufferStream.Clear();
				break;
			}

			curLength = bufferStream.GetBitLength();
		}

		RUDPHeader header{ channelId, outSeqNum++, curLength, 0, curLength };
		header.Write(packetStream);
		packetStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));

		for (; curIdx < goIdx; ++curIdx)
		{
			gameObjects[curIdx]->Write(packetStream);
		}

		ret.push_back(make_shared<Packet>(packetStream.GetBufferPtr(), packetStream.GetByteLength()));
		packetStream.Clear();
	}

	return ret;
}

vector<shared_ptr<Packet>> RUDPPacketizer::PacketizeReliable(
	const uint8_t channelId,
	uint8_t& outSeqNum, 
	const PacketType packetType, 
	OutputMemoryBitStream& inStream)
{
	return vector<shared_ptr<Packet>>();
}
