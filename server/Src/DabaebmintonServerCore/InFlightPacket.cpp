#include "ServerPCH.h"
#include "InFlightPacket.h"

#include "DeliveryNotificationManager.h"

InFlightPacket::InFlightPacket(PacketSequenceNumber inSequenceNumber) :
	mSequenceNumber(inSequenceNumber),
	mTimeDispatched(system_clock::now())
{
}

void InFlightPacket::SetTransmissionData(int inKey, TransmissionDataPtr inTransmissionData)
{
	mTransmissionDataMap[inKey] = inTransmissionData;
}

const TransmissionDataPtr InFlightPacket::GetTransmissionData(int inKey) const
{
	auto it = mTransmissionDataMap.find(inKey);
	return (it != mTransmissionDataMap.end()) ? it->second : nullptr;
}

void InFlightPacket::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	for (auto& [key, transmissionData] : mTransmissionDataMap)
	{
		transmissionData->HandleDeliveryFailure(inDeliveryNotificationManager);
	}
}

void InFlightPacket::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	for (auto& [key, transmissionData] : mTransmissionDataMap)
	{
		transmissionData->HandleDeliverySuccess(inDeliveryNotificationManager);
	}
}