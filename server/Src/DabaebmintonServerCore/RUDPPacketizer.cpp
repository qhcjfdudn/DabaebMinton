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

	OutputMemoryBitStream curStream, nextStream;
	RUDPHeader header{ channelId, outSeqNum };

	header.Write(curStream);
	curStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
	header.Write(nextStream);
	nextStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));

	for (auto gameObject : gameObjects)
	{
		gameObject->Write(nextStream);
		if (nextStream.GetByteLength() > Constant::MAX_PACKET_SIZE)
		{
			ret.push_back(make_shared<Packet>(curStream.GetBufferPtr(), curStream.GetByteLength()));

			curStream.Clear();
			nextStream.Clear();

			header = { channelId, ++outSeqNum };
			
			header.Write(curStream);
			curStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
			header.Write(nextStream);
			nextStream.WriteBits(&packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
			
			gameObject->Write(nextStream);
		}
		gameObject->Write(curStream);
	}

	ret.push_back(make_shared<Packet>(curStream.GetBufferPtr(), curStream.GetByteLength()));
	++outSeqNum;

	return ret;
}

vector<shared_ptr<Packet>> RUDPPacketizer::PacketizeReliable(
	const int channelId,
	uint8_t& outSeqNum, 
	const PacketType packetType, 
	const OutputMemoryBitStream& inStream)
{
	return vector<shared_ptr<Packet>>();
}
