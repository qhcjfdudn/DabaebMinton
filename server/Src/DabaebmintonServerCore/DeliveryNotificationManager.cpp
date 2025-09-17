#include "ServerPCH.h"
#include "DeliveryNotificationManager.h"

#include "Constant.h"

void DeliveryNotificationManager::ProcessTimedOutPackets()
{
	const auto packetTimeOutInterval = duration<float>{ Constant::PACKET_TIME_OUT };
	const auto timedOut = system_clock::now() - duration_cast<system_clock::duration>(packetTimeOutInterval);

	while (mInFlightPackets.empty() == false)
	{
		auto inFlightPacket = mInFlightPackets.front();
		if (inFlightPacket->GetTimeDispatched() < timedOut)
		{
			spdlog::warn("[DeliveryNotificationManager::ProcessTimedOutPackets] Packet timed out. Seq#: {}", inFlightPacket->GetSequenceNumber());
			mInFlightPackets.pop_front();
			HandleDeliveryFailure(inFlightPacket);
		}
		else break;
	}
}

InFlightPacketPtr DeliveryNotificationManager::GetNextInFlightPacket()
{
	++_DispatchedPacketCount;
	auto inFlightPacket = make_shared<InFlightPacket>(mNextOutgoingSequenceNumber++);
	mInFlightPackets.emplace_back(inFlightPacket);
    return mInFlightPackets.back();
}

void DeliveryNotificationManager::HandleDeliveryFailure(InFlightPacketPtr inFlightPacket)
{
	++_DroppedPacketCount;
	inFlightPacket->HandleDeliveryFailure(this);
}

void DeliveryNotificationManager::HandleDeliverySuccess(InFlightPacketPtr inFlightPacket)
{
	++_DeliveredPacketCount;
	inFlightPacket->HandleDeliverySuccess(this);
}
