#include "ServerPCH.h"
#include "DeliveryNotificationManager.h"

InFlightPacketPtr DeliveryNotificationManager::GetNextInFlightPacket()
{
	++_DispatchedPacketCount;
	auto inFlightPacket = make_shared<InFlightPacket>(mNextOutgoingSequenceNumber++);
	mInFlightPackets.emplace_back(inFlightPacket);
    return mInFlightPackets.back();
}
