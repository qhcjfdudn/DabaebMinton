#pragma once

#include "TransmissionData.h"

using PacketSequenceNumber = uint16_t;

class InFlightPacket
{
public:

	InFlightPacket(PacketSequenceNumber inSequenceNumber);

	PacketSequenceNumber		GetSequenceNumber() const { return mSequenceNumber; }
	system_clock::time_point	GetTimeDispatched() const { return mTimeDispatched; }

	void SetTransmissionData(int inKey, TransmissionDataPtr	inTransmissionData);

	const TransmissionDataPtr GetTransmissionData(int inKey) const;

	void HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const;
	void HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const;

private:
	PacketSequenceNumber		mSequenceNumber;
	system_clock::time_point	mTimeDispatched;

	unordered_map< int, TransmissionDataPtr >	mTransmissionDataMap;
};

using InFlightPacketPtr = shared_ptr< InFlightPacket>;