#include "ServerPCH.h"
#include "RUDPPacketizer.h"

#include "RUDPHeader.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"
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
	
	constexpr size_t MAX_PAYLOAD_BIT_SIZE = (Constant::MAX_PACKET_SIZE << 3) - RUDPHeader::CountsHeaderBits();

	OutputMemoryBitStream packetStream, bufferStream;
	packetStream.Reserve(Constant::MAX_PACKET_SIZE << 3);
	bufferStream.Reserve(Constant::MAX_PACKET_SIZE << 3);

	int curIdx = 0;
	int goIdx = 0;
	int goSize = static_cast<int>(gameObjects.size());

	while (goIdx < goSize)
	{
		bufferStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
		uint16_t curLength = GetRequiredBits(static_cast<int>(PacketType::PT_Max));

		for (; goIdx < goSize; ++goIdx)
		{
			gameObjects[goIdx]->Write(bufferStream);
			if (bufferStream.GetBitLength() > MAX_PAYLOAD_BIT_SIZE)
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
			gameObjects[curIdx]->Write(packetStream);

		ret.push_back(make_shared<Packet>(packetStream.GetBufferPtr(), packetStream.GetByteLength()));
		packetStream.Clear();
	}

	return ret;
}

vector<shared_ptr<Packet>> RUDPPacketizer::PacketizeReliable(
	const uint8_t channelId,
	uint8_t& outSeqNum,
	const PacketType packetType, 
	OutputMemoryBitStream& srcStream)
{
	// PacketType을 Payload에 포함
	OutputMemoryBitStream packetStream;
	packetStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
	packetStream.WriteBits(srcStream.GetBufferPtr(), srcStream.GetBitLength());

	InputMemoryBitStream payloadStream{ packetStream.GetBufferPtr(), packetStream.GetBitLength() };
	packetStream.Clear();

	// Packet을 MTU만큼 잘라 vector<Packet> 생성
	vector<shared_ptr<Packet>> ret;
	uint32_t offset = 0;
	const uint32_t totalBitLength = static_cast<uint32_t>(payloadStream.GetBitLength());

	constexpr size_t MAX_PAYLOAD_BIT_SIZE = (Constant::MAX_PACKET_SIZE << 3) - RUDPHeader::CountsHeaderBits();
	constexpr size_t MAX_PAYLOAD_BYTE_SIZE = (MAX_PAYLOAD_BIT_SIZE + 7) >> 3;

	unsigned char payloadChunk[MAX_PAYLOAD_BYTE_SIZE];
	
	while (offset < totalBitLength)
	{
		uint16_t dataBitLen = min(totalBitLength - offset, static_cast<uint32_t>(MAX_PAYLOAD_BIT_SIZE));
		RUDPHeader header = { channelId, outSeqNum++, totalBitLength, offset, dataBitLen };

		header.Write(packetStream);

		payloadStream.ReadBits(payloadChunk, dataBitLen);
		packetStream.WriteBits(payloadChunk, dataBitLen);

		ret.push_back(make_shared<Packet>(packetStream.GetBufferPtr(), packetStream.GetByteLength()));
		packetStream.Clear();

		offset += dataBitLen;
	}

	return ret;
}
