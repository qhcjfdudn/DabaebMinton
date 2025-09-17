#pragma once

#include "InFlightPacket.h"

class DeliveryNotificationManager
{
public:
	void ProcessTimedOutPackets();

	uint32_t GetDroppedPacketCount()	const { return _DroppedPacketCount; }
	uint32_t GetDeliveredPacketCount()	const { return _DeliveredPacketCount; }
	uint32_t GetDispatchedPacketCount()	const { return _DispatchedPacketCount; }

	InFlightPacketPtr GetNextInFlightPacket();

	const deque<InFlightPacketPtr>& GetInFlightPackets() const { return mInFlightPackets; }

private:
	void HandleDeliveryFailure(InFlightPacketPtr inFlightPacket);
	void HandleDeliverySuccess(InFlightPacketPtr inFlightPacket);

	PacketSequenceNumber mNextOutgoingSequenceNumber{ 1 };
	PacketSequenceNumber mNextExpectedSequenceNumber{ 1 };

	deque<InFlightPacketPtr> mInFlightPackets;

	uint32_t _DeliveredPacketCount{ 0 };
	uint32_t _DroppedPacketCount{ 0 };
	uint32_t _DispatchedPacketCount{ 0 };
};
