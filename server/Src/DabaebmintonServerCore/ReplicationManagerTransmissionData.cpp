#include "ServerPCH.h"
#include "ReplicationManagerTransmissionData.h"

#include "PacketType.h"
#include "ReplicationAction.h"
#include "NetworkManagerServer.h"
#include "DeliveryNotificationManager.h"
#include "GameObject.h"

void ReplicationManagerTransmissionData::AddTransmission(NetworkId_t inNetworkId, ReplicationAction inAction, uint8_t inState)
{
	for( const auto& transmission: mTransmissions )
	{
		assert( inNetworkId != transmission.GetNetworkId() );
	}

	mTransmissions.emplace_back(inNetworkId, inAction, inState);
}

void ReplicationManagerTransmissionData::HandleDeliveryFailure(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//run through the transmissions
	for (const ReplicationTransmission& rt : mTransmissions)
	{
		//is it a create? then we have to redo the create.
		int networkId = rt.GetNetworkId();

		switch (rt.GetAction())
		{
		case ReplicationAction::RA_Create:
			HandleCreateDeliveryFailure(networkId);
			break;
		case ReplicationAction::RA_Update:
			HandleUpdateStateDeliveryFailure(networkId, rt.GetState(), inDeliveryNotificationManager);
			break;
		case ReplicationAction::RA_Destroy:
			HandleDestroyDeliveryFailure(networkId);
			break;
		}

	}
}

void ReplicationManagerTransmissionData::HandleDeliverySuccess(DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//run through the transmissions, if any are Destroyed then we can remove this network id from the map
	for (const ReplicationTransmission& rt : mTransmissions)
	{
		switch (rt.GetAction())
		{
		case ReplicationAction::RA_Create:
			HandleCreateDeliverySuccess(rt.GetNetworkId());
			break;
		case ReplicationAction::RA_Destroy:
			HandleDestroyDeliverySuccess(rt.GetNetworkId());
			break;
		}
	}
}

void ReplicationManagerTransmissionData::HandleCreateDeliveryFailure(int inNetworkId) const
{
	//does the object still exist? it might be dead, in which case we don't resend a create
	GameObject* gameObject = NetworkManagerServer::GetInstance().GetGameObject(inNetworkId);
	if (gameObject)
	{
		_replicationManagerServer->ReplicateCreate(inNetworkId, gameObject->GetAllStateMask());
	}
}

void ReplicationManagerTransmissionData::HandleDestroyDeliveryFailure(int inNetworkId) const
{
	_replicationManagerServer->ReplicateDestroy(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleUpdateStateDeliveryFailure(int inNetworkId, uint8_t inState, DeliveryNotificationManager* inDeliveryNotificationManager) const
{
	//does the object still exist? it might be dead, in which case we don't resend an update
	if (NetworkManagerServer::GetInstance().GetGameObject(inNetworkId))
	{
		//look in all future in flight packets, in all transmissions
		//remove written state from dirty state
		for (const auto& inFlightPacket : inDeliveryNotificationManager->GetInFlightPackets())
		{
			int key = static_cast<int>(PacketType::PT_ReplicationData);
			ReplicationManagerTransmissionDataPtr rmtdp = std::static_pointer_cast<ReplicationManagerTransmissionData>(inFlightPacket->GetTransmissionData(key));

			for (const ReplicationTransmission& otherRT : rmtdp->mTransmissions)
			{
				inState &= ~otherRT.GetState();
			}
		}

		//if there's still any dirty state, mark it
		if (inState)
		{
			_replicationManagerServer->SetStateDirty(inNetworkId, inState);
		}
	}
}

void ReplicationManagerTransmissionData::HandleCreateDeliverySuccess(int inNetworkId) const
{
	//we've received an ack for the create, so we can start sending as only an update
	_replicationManagerServer->HandleCreateAckd(inNetworkId);
}

void ReplicationManagerTransmissionData::HandleDestroyDeliverySuccess(int inNetworkId) const
{
	_replicationManagerServer->RemoveFromReplication(inNetworkId);
}