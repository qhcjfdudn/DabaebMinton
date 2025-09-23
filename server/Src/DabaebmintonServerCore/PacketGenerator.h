#pragma once

#include "PacketType.h"
#include "InFlightPacket.h"
#include "OutputMemoryBitStream.h"
#include "GameObject.h"

class ReplicationManager;
class ClientProxy;

class PacketGenerator
{
public:
	
	PacketGenerator(
		DeliveryNotificationManager* deliveryNotificationManager, 
		ReplicationManager* replicationManager, 
		const PacketType packetType);

	PacketType GetPacketType() const { return _packetType; }
	OutputMemoryBitStream& GetLastStream();
	vector<OutputMemoryBitStream>& GetAllStreams() { return _streams; }
	InFlightPacketPtr GetInFlightPacket();

	OutputMemoryBitStream& GenerateNewPacketIfExceedExtraPayloadSize(const size_t sizeToAdd);
	bool isOverflowed(size_t sizeToAdd) const;

private:
	OutputMemoryBitStream& GetNextPacket();

	const PacketType _packetType;
	vector<OutputMemoryBitStream> _streams;
	InFlightPacketPtr _inFlightPacket;

	DeliveryNotificationManager* _deliveryNotificationManager;
	ReplicationManager* _replicationManager;
};
