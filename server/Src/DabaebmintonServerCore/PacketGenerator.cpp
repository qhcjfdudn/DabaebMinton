#include "ServerPCH.h"
#include "PacketGenerator.h"

#include "DeliveryNotificationManager.h"
#include "ReplicationManagerTransmissionData.h"
#include "ClientInfo.h"

#include "GetRequiredBits.h"

PacketGenerator::PacketGenerator(DeliveryNotificationManager* deliveryNotificationManager, ReplicationManager* replicationManager, const PacketType packetType) :
	_deliveryNotificationManager{ deliveryNotificationManager },
	_replicationManager{ replicationManager },
	_packetType{ packetType }
{
	GetNextPacket();
}

OutputMemoryBitStream& PacketGenerator::GetLastStream()
{
	return _streams[_streams.size() - 1];
}

InFlightPacketPtr PacketGenerator::GetInFlightPacket()
{
	return _inFlightPacket;
}

OutputMemoryBitStream& PacketGenerator::GenerateNewPacketIfExceedExtraPayloadSize(const size_t sizeToAdd)
{
	auto& stream = GetLastStream();
	if (isOverflowed(sizeToAdd) == false)
	{
		return stream;
	}

	auto& newStream = GetNextPacket();

	if (isOverflowed(sizeToAdd))
	{
		spdlog::error("[PacketGenerator::GenerateNewPacketIfExceedExtraPayloadSize] sizeToAdd {} is too large to fit in an empty packet!", sizeToAdd);
		assert(false);
	}

	return newStream;
}

bool PacketGenerator::isOverflowed(size_t sizeToAdd) const
{
	auto& stream = _streams[_streams.size() - 1];
	return stream.GetBitLength() + sizeToAdd > (Constant::MAX_PACKET_SIZE << 3);
}

OutputMemoryBitStream& PacketGenerator::GetNextPacket()
{
	_inFlightPacket = _deliveryNotificationManager->GetNextInFlightPacket();

	_streams.emplace_back();
	auto& stream = GetLastStream();
	stream.WriteBits(&_packetType, GetRequiredBits(static_cast<int>(PacketType::PT_Max)));
	stream.Write(_inFlightPacket->GetSequenceNumber());
	// deliveryNotificationManager에 pendingAck가 있다면 여기에 써줄 것

	
	if (_packetType == PacketType::PT_ReplicationData)
	{
		int key = static_cast<int>(PacketType::PT_ReplicationData);
		auto replTransData = make_shared<ReplicationManagerTransmissionData>(_replicationManager);
		_inFlightPacket->SetTransmissionData(key, replTransData);
	}

	return stream;
}
